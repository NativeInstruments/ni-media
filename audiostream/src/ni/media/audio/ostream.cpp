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

#include <ni/media/audio/ostream.h>
#include <ni/media/audio/sink.h>
#include <ni/media/iostreams/stream_buffer.h>

#include <array>
#include <cstdint>

namespace audio
{

//----------------------------------------------------------------------------------------------------------------------

ostream::ostream()
: std::ostream( nullptr )
, m_info( new info_type() )
{
}

//----------------------------------------------------------------------------------------------------------------------

template <class AudioSink>
ostream::ostream( AudioSink sink )
: std::ostream( nullptr )
, m_info( make_info( sink.info() ) )
, m_streambuf( make_stream_buffer( std::move( sink ) ) )
{
    std::ostream::rdbuf( m_streambuf.get() );
}

template ostream::ostream( wav_file_sink );

//----------------------------------------------------------------------------------------------------------------------

ostream::ostream( ostream&& other )
: std::ostream( std::move( other ) )
, m_info( std::move( other.m_info ) )
, m_streambuf( std::move( other.m_streambuf ) )
{
    std::ostream::set_rdbuf( m_streambuf.get() );
}

//----------------------------------------------------------------------------------------------------------------------

auto ostream::operator=( ostream&& other ) -> ostream&
{
    std::ostream::operator=( std::move( other ) );
    m_info                = std::move( other.m_info );
    m_streambuf           = std::move( other.m_streambuf );
    std::ostream::set_rdbuf( m_streambuf.get() );
    return *this;
}

//----------------------------------------------------------------------------------------------------------------------

auto ostream::write( const char_type* s, std::streamsize n ) -> ostream&
{
    std::ostream::write( s, n );
    return *this;
}

//----------------------------------------------------------------------------------------------------------------------

auto ostream::sample_tellp() -> pos_type
{
    return tellp() / m_info->bytes_per_sample();
}

//----------------------------------------------------------------------------------------------------------------------

auto ostream::frame_tellp() -> pos_type
{
    return tellp() / m_info->bytes_per_frame();
}

//----------------------------------------------------------------------------------------------------------------------

auto ostream::info() const -> const info_type&
{
    return *m_info;
}

} // namespace audio
