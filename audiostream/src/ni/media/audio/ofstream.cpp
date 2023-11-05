//
// Copyright (c) 2017-2019 Native Instruments GmbH, Berlin
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

#include <ni/media/audio/iotools.h>
#include <ni/media/audio/ofstream.h>

#include <ni/media/audio/sink.h>
#include <ni/media/iostreams/stream_buffer.h>

#include <boost/predef.h>

namespace audio
{

//----------------------------------------------------------------------------------------------------------------------

ofstream::ofstream()
: ostream( nullptr, std::make_unique<info_type>() )
{
}

//----------------------------------------------------------------------------------------------------------------------

ofstream::ofstream( ofstream&& other )
: ostream( std::move( other ) )
{
}

//----------------------------------------------------------------------------------------------------------------------

ofstream::ofstream( const std::string& file, const ofstream::info_type& info )
: ofstream()
{
    // the container doesn't work because the file doesn't exist yet.
    auto container = ofstream_container( file );
    if ( !container )
        throw std::runtime_error( "Unsupported file extension" );

    *this = ofstream( file, info, *container );
}

//----------------------------------------------------------------------------------------------------------------------

ofstream::ofstream( const std::string&            file,
                    const ofstream::info_type&    info,
                    ofstream_info::container_type container,
                    size_t                        stream_index )
: ofstream()
{
    using container_type = ofstream_info::container_type;

    if ( stream_index != 0 )
        throw std::runtime_error( "Unsupported stream index" );

    auto make_ofstream = []( auto&& sink ) {
        auto info = sink.info();
        return ofstream( make_stream_buffer( std::move( sink ) ),
                         std::make_unique<decltype( info )>( std::move( info ) ) );
    };

    switch ( container )
    {
#if NIMEDIA_ENABLE_AIFF_ENCODING
        case container_type::aiff:
            *this = make_ofstream( aiff_file_sink( info, file ) );
            break;
#endif
#if NIMEDIA_ENABLE_FLAC_ENCODING
        case container_type::flac:
            *this = make_ofstream( flac_file_sink( info, file ) );
            break;
#endif
#if NIMEDIA_ENABLE_WAV_ENCODING
        case container_type::wav:
            *this = make_ofstream( wav_file_sink( info, file ) );
            break;
#endif
    }
}

//----------------------------------------------------------------------------------------------------------------------

ofstream& ofstream::operator=( ofstream&& other )
{
    ostream::operator=( std::move( other ) );
    return *this;
}

//----------------------------------------------------------------------------------------------------------------------

ofstream::ofstream( std::unique_ptr<streambuf> sb, std::unique_ptr<ofstream::info_type> info )
: ostream( std::move( sb ), std::move( info ) )
{
}

//----------------------------------------------------------------------------------------------------------------------

auto ofstream::info() const -> const info_type&
{
    return static_cast<const info_type&>( ostream::info() );
}

} // namespace audio
