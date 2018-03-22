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
#include <ni/media/iostreams/stream_buffer.h>

namespace audio
{

//----------------------------------------------------------------------------------------------------------------------

ifvectorstream::ifvectorstream()
: ifstream( nullptr, std::make_unique<ifvectorstream::info_type>() )
{
}

//----------------------------------------------------------------------------------------------------------------------

ifvectorstream::ifvectorstream( std::unique_ptr<streambuf> sb, std::unique_ptr<ifvectorstream::info_type> info )
: ifstream( std::move( sb ), std::move( info ) )
{
}

//----------------------------------------------------------------------------------------------------------------------

ifvectorstream::ifvectorstream( std::vector<char> vec, const info_type& info )
: ifvectorstream( make_stream_buffer( container_source<std::vector<char>, info_type>( std::move( vec ), info ) ),
                  make_info( info ) )
{
}

//----------------------------------------------------------------------------------------------------------------------

ifvectorstream::ifvectorstream( std::vector<char> vec, ifstream_info::container_type container )
{
    using container_type = ifstream_info::container_type;

    auto make_ifvectorstream = []( auto&& source ) {
        auto info = source.info();
        return ifvectorstream( make_stream_buffer( std::move( source ) ),
                               std::make_unique<decltype( info )>( std::move( info ) ) );
    };

    switch ( container )
    {
#if NIMEDIA_ENABLE_AIFF_DECODING
        case container_type::aiff:
            *this = make_ifvectorstream( aiff_vector_source( std::move( vec ) ) );
            break;
#endif
#if NIMEDIA_ENABLE_WAV_DECODING
        case container_type::wav:
            *this = make_ifvectorstream( wav_vector_source( std::move( vec ) ) );
            break;
#endif
        default:
            throw std::runtime_error( "Unsupported container_type" );
    }
}

//----------------------------------------------------------------------------------------------------------------------

ifvectorstream::ifvectorstream( ifstream&& other )
{
    if ( other.good() )
    {
        other.frame_seekg( 0 );
        auto beg  = std::istreambuf_iterator<char>( other.rdbuf() );
        auto end  = std::istreambuf_iterator<char>();
        auto vec  = std::vector<char>( beg, end );
        auto info = other.info();
        info.num_frames( vec.size() / info.bytes_per_frame() );
        *this = ifvectorstream( std::move( vec ), info );
    }
}

//----------------------------------------------------------------------------------------------------------------------

ifvectorstream::ifvectorstream( ifvectorstream&& other )
: ifstream( std::move( other ) )
{
}

//----------------------------------------------------------------------------------------------------------------------

ifvectorstream& ifvectorstream::operator=( ifvectorstream&& other )
{
    ifstream::operator=( std::move( other ) );
    return *this;
}


} // namespace pcm
