//
// Copyright (c) 2017-2023 Native Instruments GmbH, Berlin
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


#include <ni/media/audio/ifvectorstream.h>
#include <ni/media/audio/istream.h>
#include <ni/media/audio/ivectorstream.h>

#include <ni/media/audio/source.h>
#include <ni/media/iostreams/stream_buffer.h>

#include <boost/predef.h>

#include <array>
#include <cstdint>

namespace audio
{

//----------------------------------------------------------------------------------------------------------------------

istream::istream( std::unique_ptr<streambuf> sb, std::unique_ptr<istream::info_type> info )
: std::istream( sb.get() )
, m_streambuf( std::move( sb ) )
, m_info( std::move( info ) )
{
}

//----------------------------------------------------------------------------------------------------------------------

istream::istream( istream&& other )
: std::istream( std::move( other ) )
, m_streambuf( std::move( other.m_streambuf ) )
, m_info( std::move( other.m_info ) )
{
    set_rdbuf( m_streambuf.get() );
}

//----------------------------------------------------------------------------------------------------------------------

auto istream::operator=( istream&& other ) -> istream&
{
    std::istream::operator=( std::move( other ) );
    m_info                = std::move( other.m_info );
    m_streambuf           = std::move( other.m_streambuf );
    set_rdbuf( m_streambuf.get() );
    return *this;
}

//----------------------------------------------------------------------------------------------------------------------

auto istream::read( char_type* s, std::streamsize n ) -> istream&
{
    std::istream::read( s, n );
    return *this;
}

//----------------------------------------------------------------------------------------------------------------------

auto istream::seekg( pos_type pos ) -> istream&
{
    std::istream::seekg( pos );
    return *this;
}

//----------------------------------------------------------------------------------------------------------------------

auto istream::sample_seekg( pos_type pos ) -> istream&
{
    return seekg( pos * m_info->bytes_per_sample() );
}

//----------------------------------------------------------------------------------------------------------------------

auto istream::frame_seekg( pos_type pos ) -> istream&
{
    return seekg( pos * m_info->bytes_per_frame() );
}

//----------------------------------------------------------------------------------------------------------------------

auto istream::seekg( off_type off, std::ios_base::seekdir dir ) -> istream&
{
    std::istream::seekg( off, dir );
    return *this;
}

//----------------------------------------------------------------------------------------------------------------------

auto istream::sample_seekg( off_type off, std::ios_base::seekdir dir ) -> istream&
{
    return seekg( off_type( off * m_info->bytes_per_sample() ), dir );
}

//----------------------------------------------------------------------------------------------------------------------

auto istream::frame_seekg( off_type off, std::ios_base::seekdir dir ) -> istream&
{
    return seekg( off_type( off * m_info->bytes_per_frame() ), dir );
}

//----------------------------------------------------------------------------------------------------------------------

auto istream::sample_gcount() const -> std::streamsize
{
    return gcount() / m_info->bytes_per_sample();
}

//----------------------------------------------------------------------------------------------------------------------

auto istream::frame_gcount() const -> std::streamsize
{
    return gcount() / m_info->bytes_per_frame();
}

//----------------------------------------------------------------------------------------------------------------------

auto istream::sample_tellg() -> pos_type
{
    return tellg() / m_info->bytes_per_sample();
}

//----------------------------------------------------------------------------------------------------------------------

auto istream::frame_tellg() -> pos_type
{
    return tellg() / m_info->bytes_per_frame();
}

//----------------------------------------------------------------------------------------------------------------------

auto istream::info() const -> const info_type&
{
    return *m_info;
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace audio
