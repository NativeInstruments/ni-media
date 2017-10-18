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

#include <ni/media/audio/ifvectorstream.h>
#include <ni/media/audio/iotools.h>

#include <ni/media/audio/source.h>

namespace audio
{

namespace
{

template <class Stream>
Stream open_vector_as( std::vector<char>&& vec, ifstream_info::container_type container )
{
    using container_type = ifstream_info::container_type;

    switch ( container )
    {
#if NIMEDIA_ENABLE_AIFF_DECODING
        case container_type::aiff:
            return Stream( aiff_vector_source( std::move( vec ) ) );
#endif
#if NIMEDIA_ENABLE_WAV_DECODING
        case container_type::wav:
            return Stream( wav_vector_source( std::move( vec ) ) );
#endif
        default:
            break;
    }

    throw std::runtime_error( "Unsupported container_type" );
}

} // namespace


//----------------------------------------------------------------------------------------------------------------------

ifvectorstream::ifvectorstream( std::vector<char> vec, const info_type& info )
{
    istream::operator=( container_source<std::vector<char>, info_type>( std::move( vec ), info ) );
}

//----------------------------------------------------------------------------------------------------------------------

ifvectorstream::ifvectorstream( std::vector<char> vec, ifstream_info::container_type container )
{
    istream::operator=( open_vector_as<istream>( std::move( vec ), container ) );
}

//----------------------------------------------------------------------------------------------------------------------

ifvectorstream& ifvectorstream::operator=( ifvectorstream&& other )
{
    istream::operator=( static_cast<istream&&>( std::move( other ) ) );
    return *this;
}


} // namespace pcm
