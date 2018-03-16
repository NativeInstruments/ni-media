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

#include <ni/media/audio/ivectorstream.h>

#include <ni/media/audio/source/container_source.h>
#include <ni/media/iostreams/stream_buffer.h>

#include <iterator>


namespace audio
{

//----------------------------------------------------------------------------------------------------------------------

ivectorstream::ivectorstream( std::unique_ptr<streambuf> sb, std::unique_ptr<ivectorstream::info_type> info )
: istream( std::move( sb ), std::move( info ) )
{
}

//----------------------------------------------------------------------------------------------------------------------

ivectorstream::ivectorstream()
: istream( nullptr, std::make_unique<info_type>() )
{
}

//----------------------------------------------------------------------------------------------------------------------

ivectorstream::ivectorstream( std::vector<char> vec, const info_type& info )
: ivectorstream( make_stream_buffer( container_source<std::vector<char>, info_type>( std::move( vec ), info ) ),
                 make_info( info ) )
{
}

//----------------------------------------------------------------------------------------------------------------------

ivectorstream::ivectorstream( istream&& other )
: ivectorstream()
{
    if ( other.good() )
    {
        other.frame_seekg( 0 );
        auto beg = std::istreambuf_iterator<char>( other.rdbuf() );
        auto end = std::istreambuf_iterator<char>();
        auto vec = std::vector<char>( beg, end );
        *this    = ivectorstream( std::move( vec ), other.info() );
    }
}

//----------------------------------------------------------------------------------------------------------------------

ivectorstream::ivectorstream( ivectorstream&& other )
: istream( std::move( other ) )
{
}

//----------------------------------------------------------------------------------------------------------------------

ivectorstream& ivectorstream::operator=( ivectorstream&& other )
{
    istream::operator=( std::move( other ) );
    return *this;
}

} // namespace audio
