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

#pragma once

#include <ni/media/audio/aiff/aiff_chunks.h>
#include <ni/media/audio/aiff/aiff_ofstream_info.h>
#include <ni/media/iostreams/device/subview.h>
#include <ni/media/iostreams/write_obj.h>


namespace detail
{

template <class Sink>
auto write_aiff_header( Sink& sink )
{
    // FORM tag
    write_obj( sink, boost::endian::big_uint32_t( aiff::tags::form ) );
    sink.file_size_offset( sink.tell() );
    write_obj( sink, boost::endian::big_uint32_t( 0 ) );
    write_obj( sink, boost::endian::big_uint32_t( aiff::tags::aiff ) );

    // COMM
    write_obj( sink, boost::endian::big_uint32_t( aiff::tags::comm ) );
    write_obj( sink, boost::endian::big_uint32_t( 18 ) );
    write_obj( sink, boost::endian::big_uint16_t( sink.info().num_channels() ) );
    sink.num_frames_offset( sink.tell() );
    write_obj( sink, boost::endian::big_uint32_t( 0 ) );
    write_obj( sink, boost::endian::big_uint16_t( sink.info().format().bitwidth() ) );
    write_obj( sink, aiff::CommonChunk{}.extSampleRate );

    // SSND
    write_obj( sink, boost::endian::big_uint32_t( aiff::tags::ssnd ) );
    sink.ssnd_size_offset( sink.tell() );
    write_obj( sink, boost::endian::big_uint32_t( 0 ) );
    write_obj( sink, aiff::SoundDataChunk{0, 0} );

    sink.header_size( sink.tell() );
}

template <class Sink>
auto close_aiff( Sink& sink )
{
    uint32_t file_size = static_cast<uint32_t>( sink.tell() );

    sink.seek( sink.file_size_offset(), BOOST_IOS::beg );
    write_obj( sink, boost::endian::big_uint32_t( file_size - 8 ) );

    sink.seek( sink.num_frames_offset(), BOOST_IOS::beg );
    uint32_t samples =
        ( file_size - sink.header_size() ) / ( sink.info().num_channels() * sink.info().format().bitwidth() / 8 );
    write_obj( sink, boost::endian::big_uint32_t( samples ) );

    sink.seek( sink.ssnd_size_offset(), BOOST_IOS::beg );
    write_obj( sink, boost::endian::big_uint32_t( file_size - sink.header_size() + 8 ) );
}

} // namespace detail

template <class Sink>
class aiff_sink : public boostext::iostreams::subview_sink<Sink>
{
    using base_type   = boostext::iostreams::subview_sink<Sink>;
    using offset_type = typename base_type::offset_type;

public:
    using info_type = audio::aiff_ofstream_info;

    template <class... Args>
    explicit aiff_sink( info_type info, Args&&... args )
    : base_type( std::forward<Args>( args )... )
    , m_info( info )
    {
        detail::write_aiff_header( *this );
        auto pos = this->tell();
        this->set_view( pos );
    }

    void close()
    {
        this->set_view( 0 );
        detail::close_aiff( *this );
        base_type::close();
        m_info = info_type();
    }

    auto info() const -> info_type
    {
        return m_info;
    }

    auto file_size_offset() const -> offset_type
    {
        return m_file_size_offset;
    }

    void file_size_offset( const offset_type off )
    {
        m_file_size_offset = off;
    }

    auto num_frames_offset() const -> offset_type
    {
        return m_num_frames_offset;
    }

    void num_frames_offset( const offset_type off )
    {
        m_num_frames_offset = off;
    }

    auto ssnd_size_offset() const -> offset_type
    {
        return m_ssnd_size_offset;
    }

    void ssnd_size_offset( const offset_type off )
    {
        m_ssnd_size_offset = off;
    }

    auto header_size() const -> uint32_t
    {
        return m_header_size;
    }

    void header_size( const uint32_t size )
    {
        m_header_size = size;
    }

private:
    info_type   m_info;
    offset_type m_file_size_offset  = 0;
    offset_type m_num_frames_offset = 0;
    offset_type m_ssnd_size_offset  = 0;
    uint32_t    m_header_size       = 0;
};
