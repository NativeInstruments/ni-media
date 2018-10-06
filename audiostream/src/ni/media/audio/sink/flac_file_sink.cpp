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

#include <ni/media/audio/sink/flac_file_sink.h>

#include <ni/media/iostreams/positioning.h>

#define FLAC__NO_DLL
#include <FLAC++/encoder.h>

//----------------------------------------------------------------------------------------------------------------------

class flac_file_sink::Impl
{
};

//----------------------------------------------------------------------------------------------------------------------

flac_file_sink::flac_file_sink() = default;

//----------------------------------------------------------------------------------------------------------------------

flac_file_sink::~flac_file_sink()
{
    close();
}

//----------------------------------------------------------------------------------------------------------------------

flac_file_sink::flac_file_sink( flac_file_sink&& ) = default;

//----------------------------------------------------------------------------------------------------------------------

flac_file_sink::flac_file_sink( const info_type& info, const std::string& path )
: m_info( info )
{
    open( path );
}

//----------------------------------------------------------------------------------------------------------------------

void flac_file_sink::open( const std::string& path )
{
    m_file = std::fopen( path.c_str(), "w" );
}

//----------------------------------------------------------------------------------------------------------------------

auto flac_file_sink::write( const char_type* s, std::streamsize n ) -> std::streamsize
{
    return {};
}

//----------------------------------------------------------------------------------------------------------------------

void flac_file_sink::close()
{
    std::fclose( m_file );
}

//----------------------------------------------------------------------------------------------------------------------

auto flac_file_sink::info() const -> info_type
{
    return m_info;
}

//----------------------------------------------------------------------------------------------------------------------