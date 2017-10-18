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

#include <iterator>


namespace audio
{
//----------------------------------------------------------------------------------------------------------------------

ivectorstream::ivectorstream( std::vector<char> vec, const istream::info_type& info )
{
    istream::operator=( container_source<std::vector<char>, istream::info_type>( std::move( vec ), info ) );
}

//----------------------------------------------------------------------------------------------------------------------

ivectorstream::ivectorstream( istream&& other )
{
    if ( other.good() )
    {
        other.frame_seekg( 0 );
        auto vec = std::vector<char>( other.info().num_bytes() );
        other.read( vec.data(), other.info().num_bytes() );
        istream::operator=( container_source<std::vector<char>, istream::info_type>( std::move( vec ), other.info() ) );
    }
}

//----------------------------------------------------------------------------------------------------------------------

ivectorstream::ivectorstream( ivectorstream&& other )
: istream( static_cast<istream&&>( std::move( other ) ) )
{
}

//----------------------------------------------------------------------------------------------------------------------

ivectorstream& ivectorstream::operator=( ivectorstream&& other )
{
    istream::operator=( static_cast<istream&&>( std::move( other ) ) );
    return *this;
}


} // namespace audio
