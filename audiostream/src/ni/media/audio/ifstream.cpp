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

#include <ni/media/audio/ifstream.h>
#include <ni/media/audio/iotools.h>

#include <ni/media/audio/source.h>

#include <boost/predef.h>

namespace audio
{

namespace
{

template <class Stream>
Stream open_file_as( const std::string& file, ifstream_info::container_type container, size_t stream_index = 0 )
{
    using container_type = ifstream_info::container_type;

    if ( container != container_type::mp4 && stream_index != 0 )
        throw std::runtime_error( "Unsupported stream index" );

    switch ( container )
    {

#if NIMEDIA_ENABLE_AIFF_DECODING
        case container_type::aiff:
            return Stream( aiff_file_source( file ) );
#endif

#if NIMEDIA_ENABLE_FLAC_DECODING
        case container_type::flac:
            return Stream( flac_file_source( file ) );
#endif

#if NIMEDIA_ENABLE_MP3_DECODING
        case container_type::mp3:
            return Stream( mp3_file_source( file ) );
#endif

#if NIMEDIA_ENABLE_MP4_DECODING
        case container_type::mp4:
            return Stream( mp4_file_source( file, stream_index ) );
#endif

#if NIMEDIA_ENABLE_OGG_DECODING
        case container_type::ogg:
            return Stream( ogg_file_source( file ) );
#endif

#if NIMEDIA_ENABLE_WAV_DECODING
        case container_type::wav:
            return Stream( wav_file_source( file ) );
#endif

#if NIMEDIA_ENABLE_WMA_DECODING
        case container_type::wma:
            return Stream( wma_file_source( file ) );
#endif
        default:
            break;
    }

    throw std::runtime_error( "Unsupported container_type" );
}

template <class Stream>
Stream open_file_as( const std::string& file )
{
    if ( auto container = ifstream_container( file ) )
        return open_file_as<Stream>( file, *container );

    throw std::runtime_error( "Unsupported file extension" );
}

} // namespace


//----------------------------------------------------------------------------------------------------------------------

ifstream::ifstream( const std::string& file )
{
    istream::operator=( open_file_as<istream>( file ) );
}

//----------------------------------------------------------------------------------------------------------------------

ifstream::ifstream( const std::string& file, ifstream_info::container_type container, size_t stream_index )
{
    istream::operator=( open_file_as<istream>( file, container, stream_index ) );
}

//----------------------------------------------------------------------------------------------------------------------

ifstream::ifstream( ifstream&& other )
: istream( static_cast<istream&&>( std::move( other ) ) )
{
}

//----------------------------------------------------------------------------------------------------------------------

ifstream& ifstream::operator=( ifstream&& other )
{
    istream::operator=( static_cast<istream&&>( std::move( other ) ) );
    return *this;
}

//----------------------------------------------------------------------------------------------------------------------

const ifstream::info_type& ifstream::info() const
{
    return static_cast<const info_type&>( istream::info() );
}

} // namespace pcm
