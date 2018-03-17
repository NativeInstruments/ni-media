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
#include <ni/media/iostreams/stream_buffer.h>

#include <boost/predef.h>

namespace audio
{

//----------------------------------------------------------------------------------------------------------------------

ifstream::ifstream()
: istream( nullptr, std::make_unique<info_type>() )
{
}

//----------------------------------------------------------------------------------------------------------------------

ifstream::ifstream( std::unique_ptr<streambuf> sb, std::unique_ptr<info_type> info )
: istream( std::move( sb ), std::move( info ) )
{
}

//----------------------------------------------------------------------------------------------------------------------

ifstream::ifstream( const std::string& file )
: ifstream()
{
#if NIMEDIA_ENABLE_ITUNES_DECODING
    if ( is_itunes_url( file ) )
    {
        auto source = avassetreader_source( file, 0 );
        auto info   = source.info();
        *this = ifstream( make_stream_buffer( std::move( source ) ), std::make_unique<info_type>( std::move( info ) ) );
        return;
    }
#endif

    auto container = ifstream_container( file );
    if ( !container )
        throw std::runtime_error( "Unsupported file extension" );

    *this = ifstream( file, *container );
}

//----------------------------------------------------------------------------------------------------------------------

ifstream::ifstream( const std::string& file, ifstream_info::container_type container, size_t stream_index )
: ifstream()
{
    using container_type = ifstream_info::container_type;

    if ( container != container_type::mp4 && stream_index != 0 )
    {
        throw std::runtime_error( "Unsupported stream index" );
    }

    auto make_ifstream = []( auto&& source ) {
        auto info = source.info();
        return ifstream( make_stream_buffer( std::move( source ) ),
                         std::make_unique<decltype( info )>( std::move( info ) ) );
    };

    switch ( container )
    {
#if NIMEDIA_ENABLE_AIFF_DECODING
        case container_type::aiff:
            *this = make_ifstream( aiff_file_source( file ) );
            break;
#endif

#if NIMEDIA_ENABLE_FLAC_DECODING
        case container_type::flac:
            *this = make_ifstream( flac_file_source( file ) );
            break;
#endif

#if NIMEDIA_ENABLE_MP3_DECODING
        case container_type::mp3:
            *this = make_ifstream( mp3_file_source( file ) );
            break;
#endif

#if NIMEDIA_ENABLE_MP4_DECODING
        case container_type::mp4:
            *this = make_ifstream( mp4_file_source( file, stream_index ) );
            break;
#endif

#if NIMEDIA_ENABLE_OGG_DECODING
        case container_type::ogg:
            *this = make_ifstream( ogg_file_source( file ) );
            break;
#endif

#if NIMEDIA_ENABLE_WAV_DECODING
        case container_type::wav:
            *this = make_ifstream( wav_file_source( file ) );
            break;
#endif

#if NIMEDIA_ENABLE_WMA_DECODING
        case container_type::wma:
            *this = make_ifstream( wma_file_source( file ) );
            break;
#endif

        default:
            throw std::runtime_error( "Unsupported container_type" );
    }
}

//----------------------------------------------------------------------------------------------------------------------

ifstream::ifstream( ifstream&& other )
: istream( std::move( other ) )
{
}

//----------------------------------------------------------------------------------------------------------------------

ifstream& ifstream::operator=( ifstream&& other )
{
    istream::operator=( std::move( other ) );
    return *this;
}

//----------------------------------------------------------------------------------------------------------------------

const ifstream::info_type& ifstream::info() const
{
    return static_cast<const info_type&>( istream::info() );
}

} // namespace pcm
