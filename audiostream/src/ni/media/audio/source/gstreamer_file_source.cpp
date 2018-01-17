//
// Copyright (c) 2017 Native Instruments GmbH, Berlin
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#include "gstreamer_file_source.h"

#include <ni/media/audio/iotools.h>
#include <ni/media/iostreams/positioning.h>

#include <boost/make_unique.hpp>

#include <algorithm>
#include <gst/app/gstappsink.h>

namespace detail
{
template <typename T, size_t s>
struct RingBuffer
{
    static constexpr size_t size = s;
    static constexpr size_t mask = ( s - 1 );
    std::array<T, s> m_buffer;
    uint64_t m_write_head = 0;
    uint64_t m_read_head  = 0;

    void push( const T* data, size_t count )
    {
        auto filled = m_write_head - m_read_head;
        auto space  = s - filled;
        assert( count <= space );

        if ( count )
        {
            auto idx     = m_write_head & mask;
            auto space   = size - idx;
            auto for_now = std::min( space, count );
            std::copy( data, data + for_now, m_buffer.data() + idx );
            m_write_head += for_now;
            push( data + for_now, count - for_now );
        }
    }

    size_t pull( T* data, size_t count )
    {
        count = std::min( count, m_write_head - m_read_head );

        if ( count )
        {
            auto idx     = m_read_head & mask;
            auto space   = size - idx;
            auto for_now = std::min( space, count );
            std::copy( m_buffer.data() + idx, m_buffer.data() + idx + for_now, data );
            m_read_head += for_now;
            return for_now + pull( data + for_now, count - for_now );
        }
        return 0;
    }

    void flush()
    {
        m_read_head = m_write_head = 0;
    }
};
}

//----------------------------------------------------------------------------------------------------------------------
gstreamer_file_source::gstreamer_file_source( const std::string&                   path,
                                              audio::ifstream_info::container_type container,
                                              size_t                               stream )
: m_pipeline( nullptr, gst_object_unref )
, m_ring_buffer( new RingBuffer() )
{
    init_gstreamer();
    setup_source( path, container );
}

//----------------------------------------------------------------------------------------------------------------------

gstreamer_file_source::~gstreamer_file_source()
{
    gst_element_set_state( m_pipeline.get(), GST_STATE_NULL );
    wait_for_async_operation();
    m_pipeline.reset();
}

//----------------------------------------------------------------------------------------------------------------------

GstState gstreamer_file_source::wait_for_async_operation()
{
    GstState current_state = GST_STATE_VOID_PENDING;
    GstState pending_state = GST_STATE_VOID_PENDING;

    while ( gst_element_get_state( m_pipeline.get(), &current_state, &pending_state, GST_MSECOND )
            == GST_STATE_CHANGE_ASYNC )
    {
        g_main_context_iteration( nullptr, FALSE );
    }

    return current_state;
}

//----------------------------------------------------------------------------------------------------------------------

void gstreamer_file_source::init_gstreamer()
{
    static bool gstreamer_initialized = false;

    if ( !std::exchange( gstreamer_initialized, true ) )
    {
        gst_init( 0, nullptr );
    }
}

//----------------------------------------------------------------------------------------------------------------------

void gstreamer_file_source::setup_source( const std::string& path, audio::ifstream_info::container_type container )
{
    GstElement* sink        = prepare_pipeline( path );
    auto        sinkpad     = gst_element_get_static_pad( sink, "sink" );
    auto        caps        = gst_pad_get_current_caps( sinkpad );
    auto        caps_struct = gst_caps_get_structure( caps, 0 );
    fill_format_info( caps_struct, container );
}

//----------------------------------------------------------------------------------------------------------------------

GstElement* gstreamer_file_source::prepare_pipeline( const std::string& path )
{
    m_pipeline.reset( gst_pipeline_new( "pipeline" ) );

    GstElement* source    = gst_element_factory_make( "filesrc", "source" );
    GstElement* decodebin = gst_element_factory_make( "decodebin", "decoder" );
    GstElement* queue     = gst_element_factory_make( "queue", "queue" );
    GstElement* sink      = gst_element_factory_make( "appsink", "sink" );

    gst_bin_add_many( GST_BIN( m_pipeline.get() ), source, decodebin, queue, sink, nullptr );
    gst_element_link_many( source, decodebin, NULL );
    gst_element_link_many( queue, sink, NULL );

    g_object_set( source, "location", path.c_str(), NULL );

    g_signal_connect( decodebin, "pad-added", G_CALLBACK( &onPadAdded ), queue );

    preroll_pipeline();
    return sink;
}

//----------------------------------------------------------------------------------------------------------------------

void gstreamer_file_source::preroll_pipeline()
{
    gst_element_set_state( m_pipeline.get(), GST_STATE_PAUSED );

    if ( wait_for_async_operation() != GST_STATE_PAUSED )
        throw std::runtime_error( "gstreamer_file_source: pipeline doesn't preroll into paused state" );

    gst_element_set_state( m_pipeline.get(), GST_STATE_PLAYING );

    if ( wait_for_async_operation() != GST_STATE_PLAYING )
        throw std::runtime_error( "gstreamer_file_source: pipeline doesn't preroll into playing state" );
}

//----------------------------------------------------------------------------------------------------------------------

void gstreamer_file_source::fill_format_info( GstStructure*                        caps_struct,
                                              audio::ifstream_info::container_type container )
{
    m_info.container( container );
    m_info.codec( audio::ifstream_info::codec_type::mp3 );
    m_info.lossless( false );

    gint64 num_frames = 0;
    if ( !gst_element_query_duration( m_pipeline.get(), GST_FORMAT_DEFAULT, &num_frames ) )
        throw std::runtime_error( "gstreamer_file_source: could not query duration from gstreamer" );

    m_info.num_frames( num_frames );

    int sample_rate = 0;
    if ( !gst_structure_get_int( caps_struct, "rate", &sample_rate ) )
        throw std::runtime_error( "gstreamer_file_source: could not query sample rate from gstreamer" );

    m_info.sample_rate( sample_rate );

    int num_channels = 0;
    if ( !gst_structure_get_int( caps_struct, "channels", &num_channels ) )
        throw std::runtime_error( "gstreamer_file_source: could not query number of channels from gstreamer" );

    m_info.num_channels( num_channels );

    m_info.format( create_runtime_format( caps_struct ) );
}

//----------------------------------------------------------------------------------------------------------------------

pcm::runtime_format gstreamer_file_source::create_runtime_format( GstStructure* caps_struct )
{
    if ( auto format = gst_structure_get_string( caps_struct, "format" ) )
    {
        pcm::number_type number_type = gst_format_char_to_number_type( format[0] );
        auto             srcDepth    = std::atol( format + 1 );
        auto             endian      = ( strcmp( format, "BE" ) == 0 ) ? pcm::big_endian : pcm::little_endian;
        return pcm::runtime_format( number_type, srcDepth, endian );
    }
    throw std::runtime_error( "gstreamer_file_source: could not get runtime format from gstreamer caps" );
}

//----------------------------------------------------------------------------------------------------------------------

pcm::number_type gstreamer_file_source::gst_format_char_to_number_type( const gchar format )
{
    if ( format == 'U' )
        return pcm::unsigned_integer;
    else if ( format == 'F' )
        return pcm::floating_point;

    return pcm::signed_integer;
}

//----------------------------------------------------------------------------------------------------------------------

void gstreamer_file_source::onPadAdded( GstElement* element, GstPad* pad, GstElement* sink )
{
    tGstPtr<GstPad> sinkpad( gst_element_get_static_pad( sink, "sink" ), gst_object_unref );
    gst_pad_link( pad, sinkpad.get() );
}

//----------------------------------------------------------------------------------------------------------------------

std::streampos gstreamer_file_source::seek( offset_type off, BOOST_IOS::seekdir way )
{
    assert( 0 == off % m_info.bytes_per_frame() );

    int64_t newPosition = 0;

    if ( way == BOOST_IOS::seekdir::_S_beg )
    {
        newPosition = off;
    }
    else if ( way == BOOST_IOS::seekdir::_S_cur )
    {
        newPosition = m_position + off;
    }
    else if ( way == BOOST_IOS::seekdir::_S_end )
    {
        int64_t end = 0;
        if ( !gst_element_query_duration( m_pipeline.get(), GST_FORMAT_BYTES, &end ) )
        {
            throw std::runtime_error(
                "gstreamer_file_source: seeking from end of file impossible - could not query duration" );
        }
        newPosition = end + off;
    }

    if ( m_position != newPosition )
    {
        m_position = newPosition;
        m_ring_buffer->flush();

        if ( !gst_element_seek_simple( m_pipeline.get(),
                                       GST_FORMAT_BYTES,
                                       ( GstSeekFlags )( GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE ),
                                       m_position ) )
        {
            throw std::runtime_error( "gstreamer_file_source: seeking failed" );
        }
    }

    return m_position;
}

//----------------------------------------------------------------------------------------------------------------------

std::streamsize gstreamer_file_source::read( char* dst, std::streamsize numBytesRequested )
{
    return recursive_read( dst, numBytesRequested );
}

//----------------------------------------------------------------------------------------------------------------------

std::streamsize gstreamer_file_source::recursive_read( char* dst, std::streamsize numBytesRequested )
{
    auto read_bytes = m_ring_buffer->pull( dst, numBytesRequested );

    dst += read_bytes;
    numBytesRequested -= read_bytes;
    m_position += read_bytes;

    if ( numBytesRequested )
    {
        tGstPtr<GstElement> sink( gst_bin_get_by_name( GST_BIN( m_pipeline.get() ), "sink" ), gst_object_unref );
        GstAppSink*         app_sink = reinterpret_cast<GstAppSink*>( sink.get() );

        tGstPtr<GstSample> sample( gst_app_sink_pull_sample( app_sink ), (GUnref) gst_sample_unref );

        if ( sample )
        {
            auto buffer = gst_sample_get_buffer( sample.get() );

            GstMapInfo mapped;

            if ( gst_buffer_map( buffer, &mapped, GST_MAP_READ ) )
            {
                auto for_now = std::min<gsize>( mapped.size, numBytesRequested );
                std::copy( mapped.data, mapped.data + for_now, dst );
                read_bytes += for_now;
                dst += for_now;
                numBytesRequested -= for_now;
                m_position += for_now;

                m_ring_buffer->push( (char*) mapped.data + for_now, mapped.size - for_now );
                gst_buffer_unmap( buffer, &mapped );

                return read_bytes + recursive_read( dst, numBytesRequested );
            }
        }
    }

    return read_bytes;
}
