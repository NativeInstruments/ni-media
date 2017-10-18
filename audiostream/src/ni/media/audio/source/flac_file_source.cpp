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

#include <ni/media/audio/source/flac_file_source.h>

#include <boost/algorithm/clamp.hpp>

#include <functional>
#include <vector>

#define FLAC__NO_DLL
#include <FLAC++/decoder.h>

//----------------------------------------------------------------------------------------------------------------------
// Implementation class

class flac_file_source::Impl
{

public:
    Impl( const std::string& path );
    std::streamsize read( char_type* s, std::streamsize n );
    std::streampos seek( boost::iostreams::stream_offset off, BOOST_IOS::seekdir way );

    audio::ifstream_info info() const
    {
        return m_info;
    }

private:
    // callback implementations
    bool writeCallbackImpl( const FLAC__Frame* frame, const FLAC__int32* const buffer[] );
    void metadataCallbackImpl( const FLAC__StreamMetadata* metadata );
    void errorCallbackImpl( FLAC__StreamDecoderErrorStatus status );

    // FLAC decoder callbacks
    static FLAC__StreamDecoderWriteStatus write_callback( const FLAC__StreamDecoder* decoder,
                                                          const FLAC__Frame*         frame,
                                                          const FLAC__int32* const   buffer[],
                                                          void*                      client_data );
    static void metadata_callback( const FLAC__StreamDecoder*  decoder,
                                   const FLAC__StreamMetadata* metadata,
                                   void*                       client_data );
    static void error_callback( const FLAC__StreamDecoder*     decoder,
                                FLAC__StreamDecoderErrorStatus status,
                                void*                          client_data );

    audio::ifstream_info m_info;

    std::vector<char_type> m_buffer;                // buffer (in bytes) used by FLAC decoder
    size_t                 m_buffer_offset;         // buffer offset of next byte to return
    bool                   m_audio_block_available; // is there data in the write buffer?
    size_t                 m_total_samples;         // total number of samples in each channel
    std::streampos         m_pos = 0;

    using DecoderPtr = std::unique_ptr<FLAC__StreamDecoder, std::function<void( FLAC__StreamDecoder* )>>;
    DecoderPtr m_decoder;
};

//----------------------------------------------------------------------------------------------------------------------

flac_file_source::Impl::Impl( const std::string& path )
: m_buffer_offset( 0 )
, m_audio_block_available( false )
, m_decoder( DecoderPtr(::FLAC__stream_decoder_new(), []( FLAC__StreamDecoder* ptr ) {
    if ( ptr )
    {
        FLAC__stream_decoder_finish( ptr );
        FLAC__stream_decoder_delete( ptr );
    }
} ) )
{
    if ( !m_decoder )
        throw std::runtime_error( "flac_file_source: Could not instantiate decoder." );

    auto status = FLAC__stream_decoder_init_file(
        m_decoder.get(), path.c_str(), write_callback, metadata_callback, error_callback, this );

    if ( status == FLAC__STREAM_DECODER_INIT_STATUS_ERROR_OPENING_FILE )
        throw std::runtime_error( "flac_file_source: Error opening file" );
    else if ( status != FLAC__STREAM_DECODER_INIT_STATUS_OK )
        throw std::runtime_error( "flac_file_source: Could not initialize FLAC decoder." );

    // read metadata
    FLAC__stream_decoder_set_metadata_respond( m_decoder.get(), FLAC__METADATA_TYPE_STREAMINFO );
    if ( !FLAC__stream_decoder_process_until_end_of_metadata( m_decoder.get() ) )
    {
        throw std::runtime_error( "FLAC: Can't decode metadata" );
    }
}

//----------------------------------------------------------------------------------------------------------------------
// process audio frames and copy from internal buffer to client buffer

std::streamsize flac_file_source::Impl::read( char_type* s, std::streamsize n )
{
    size_t requested = size_t( n ); // total bytes requested
    size_t delivered = 0;           // accumulator for bytes read

    while ( delivered < requested )
    {
        // if there is no audio buffered and we can't fetch another frame, break
        if ( !m_audio_block_available && !FLAC__stream_decoder_process_single( m_decoder.get() ) )
            break;

        // copy at most (requested - delivered) bytes to s
        size_t to_copy = std::min( m_buffer.size() - m_buffer_offset, requested - delivered );
        s              = std::copy_n( m_buffer.begin() + m_buffer_offset, to_copy, s );
        delivered += to_copy;
        m_buffer_offset += to_copy;

        if ( m_buffer_offset == m_buffer.size() )
        {
            m_audio_block_available = false;
            m_buffer_offset         = 0;
        }
    }

    m_pos += delivered;
    return delivered;
}

//----------------------------------------------------------------------------------------------------------------------

std::streampos flac_file_source::Impl::seek( boost::iostreams::stream_offset off, BOOST_IOS::seekdir way )
{
    const std::streampos beg = 0;
    const std::streampos end = info().num_bytes();

    // Determine absolute byte pos
    std::streampos pos;
    if ( way == BOOST_IOS::beg )
        pos = off;
    else if ( way == BOOST_IOS::cur )
        pos = m_pos + boost::iostreams::stream_offset_to_streamoff( off );
    else if ( way == BOOST_IOS::end )
        pos = end + boost::iostreams::stream_offset_to_streamoff( off );
    else
        throw BOOST_IOSTREAMS_FAILURE( "bad seek direction" );

    pos = boost::algorithm::clamp( pos, beg, end );

    FLAC__uint64 frame_pos   = pos / m_info.bytes_per_frame();
    FLAC__uint64 byte_offset = pos % m_info.bytes_per_frame();

    if ( FLAC__stream_decoder_seek_absolute( m_decoder.get(), frame_pos ) )
    {
        m_pos           = pos;
        m_buffer_offset = size_t( byte_offset );
    }
    return m_pos;
}

//----------------------------------------------------------------------------------------------------------------------
// FLAC callback - "write" (copy) the FLAC frame of samples to our internal buffer

bool flac_file_source::Impl::writeCallbackImpl( const FLAC__Frame* frame, const FLAC__int32* const buffer[] )
{

    auto channels     = m_info.num_channels();
    auto sample_bytes = m_info.bytes_per_sample();
    auto total_bytes  = frame->header.blocksize * sample_bytes * channels;

    if ( m_buffer.size() != total_bytes )
        m_buffer.resize( total_bytes );

    // copy from FLAC buffer (samples) to internal buffer (bytes)
    for ( size_t u = 0; u < frame->header.blocksize; ++u )
    {
        for ( size_t channel = 0; channel < channels; ++channel )
        {

            // Interleave samples
            auto sample       = buffer[channel][u];
            auto sample_index = sample_bytes * ( u * channels + channel );

            // Copy sample bytes into writeBuffer
            for ( size_t b = 0; b < sample_bytes; ++b )
            {
                char_type c                = ( sample >> ( 8 * b ) ) & 0xFF;
                m_buffer[sample_index + b] = c;
            }
        }
    }

    m_audio_block_available = true;
    return true;
}

//----------------------------------------------------------------------------------------------------------------------
// FLAC callback to write metadata into audio::stream_info

void flac_file_source::Impl::metadataCallbackImpl( const FLAC__StreamMetadata* metadata )
{
    if ( metadata->type != FLAC__METADATA_TYPE_STREAMINFO )
        return;
    auto meta_info = metadata->data.stream_info;

    assert( meta_info.min_blocksize == meta_info.max_blocksize ); // can only handle constant blocksize

    m_total_samples = size_t( meta_info.total_samples );

    // fill and set audio::stream_info
    audio::ifstream_info info;
    info.lossless( true );
    info.codec( audio::ifstream_info::codec_type::flac );
    info.container( audio::ifstream_info::container_type::flac );
    info.num_channels( meta_info.channels );
    info.num_frames( m_total_samples );
    info.sample_rate( meta_info.sample_rate );
    info.format( pcm::format( pcm::signed_integer, meta_info.bits_per_sample, pcm::little_endian ) );
    m_info = info;
}

//----------------------------------------------------------------------------------------------------------------------

void flac_file_source::Impl::errorCallbackImpl( FLAC__StreamDecoderErrorStatus )
{
    throw std::runtime_error( "flac_file_source: Error decoding FLAC file" );
}

//----------------------------------------------------------------------------------------------------------------------

::FLAC__StreamDecoderWriteStatus flac_file_source::Impl::write_callback( const FLAC__StreamDecoder*,
                                                                         const FLAC__Frame*       frame,
                                                                         const FLAC__int32* const buffer[],
                                                                         void*                    client_data )
{
    if ( !static_cast<Impl*>( client_data )->writeCallbackImpl( frame, buffer ) )
    {
        return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
    }

    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

//----------------------------------------------------------------------------------------------------------------------

void flac_file_source::Impl::metadata_callback( const FLAC__StreamDecoder*,
                                                const FLAC__StreamMetadata* metadata,
                                                void*                       client_data )
{
    static_cast<Impl*>( client_data )->metadataCallbackImpl( metadata );
}

//----------------------------------------------------------------------------------------------------------------------

void flac_file_source::Impl::error_callback( const FLAC__StreamDecoder*,
                                             FLAC__StreamDecoderErrorStatus status,
                                             void*                          client_data )
{
    static_cast<Impl*>( client_data )->errorCallbackImpl( status );
}

//----------------------------------------------------------------------------------------------------------------------

flac_file_source::flac_file_source() = default;

//----------------------------------------------------------------------------------------------------------------------

flac_file_source::~flac_file_source() = default;

//----------------------------------------------------------------------------------------------------------------------

flac_file_source::flac_file_source( flac_file_source&& ) = default;

//----------------------------------------------------------------------------------------------------------------------

flac_file_source::flac_file_source( const std::string& path )
{
    open( path );
}

//----------------------------------------------------------------------------------------------------------------------

void flac_file_source::open( const std::string& path )
{
    m_impl.reset( new Impl( path ) );
}

//----------------------------------------------------------------------------------------------------------------------

void flac_file_source::close()
{
    m_impl.reset();
}

//----------------------------------------------------------------------------------------------------------------------

auto flac_file_source::read( char_type* s, std::streamsize n ) -> std::streamsize
{
    if ( m_impl )
        return m_impl->read( s, n );
    else
        throw std::runtime_error( "flac_file_source: reading from closed file" );
}

//----------------------------------------------------------------------------------------------------------------------

auto flac_file_source::seek( boost::iostreams::stream_offset off, BOOST_IOS::seekdir way ) -> std::streampos
{
    if ( m_impl )
        return m_impl->seek( off, way );
    else
        throw std::runtime_error( "flac_file_source: seeking in closed file" );
}

//----------------------------------------------------------------------------------------------------------------------

flac_file_source::info_type flac_file_source::info() const
{
    return m_impl ? m_impl->info() : info_type{};
}
