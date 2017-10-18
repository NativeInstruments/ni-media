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

#include <ni/media/audio/iotools.h>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/predef.h>

namespace audio
{

namespace
{


auto ifstream_map() -> const std::map<std::string, ifstream_info::container_type>&
{
    using container_type = ifstream_info::container_type;

    // clang-format off
    static const std::map<std::string, container_type> map
    {
#if NIMEDIA_ENABLE_AIFF_DECODING
        {".aif", container_type::aiff},
        {".aiff", container_type::aiff},
        {".aifc", container_type::aiff},
#endif
#if NIMEDIA_ENABLE_FLAC_DECODING
        {".flac", container_type::flac},
#endif
#if NIMEDIA_ENABLE_MP3_DECODING
        {".mp3", container_type::mp3},
#endif
#if NIMEDIA_ENABLE_MP4_DECODING
        {".mp4", container_type::mp4},
        {".m4a", container_type::mp4},
#endif
#if NIMEDIA_ENABLE_OGG_DECODING
        {".ogg", container_type::ogg},
#endif
#if NIMEDIA_ENABLE_WAV_DECODING
        {".wav", container_type::wav},
        {".wave", container_type::wav},
#endif
#if NIMEDIA_ENABLE_WMA_DECODING
        {".wma", container_type::wma},
#endif
    };
    // clang-format on
    return map;
}

//----------------------------------------------------------------------------------------------------------------------

auto ofstream_map() -> const std::map<std::string, ofstream_info::container_type>&
{
    using container_type = ofstream_info::container_type;

    // clang-format off
    static const std::map<std::string, container_type> map
    {
#if NIMEDIA_ENABLE_WAV_ENCODING
        {".wav", container_type::wav},
        {".wave", container_type::wav},
#endif
    };
    // clang-format on

    return map;
}

template <class Map>
auto container_of( const boost::filesystem::path& path, const Map& map ) -> boost::optional<typename Map::mapped_type>
{
    if ( is_regular_file( path ) && path.has_extension() )
    {
        auto it = map.find( boost::to_lower_copy( path.extension().string() ) );
        if ( it != map.end() )
            return it->second;
    }

    return boost::none;
}


} // namespace

//----------------------------------------------------------------------------------------------------------------------

auto ifstream_container( const boost::filesystem::path& path ) -> boost::optional<ifstream_info::container_type>
{
    return container_of( path, ifstream_map() );
}


//----------------------------------------------------------------------------------------------------------------------

auto ofstream_container( const boost::filesystem::path& path ) -> boost::optional<ofstream_info::container_type>
{
    return container_of( path, ofstream_map() );
}

//----------------------------------------------------------------------------------------------------------------------

bool can_read_file( const boost::filesystem::path& path )
{
    return ifstream_container( path ) != boost::none;
}

//----------------------------------------------------------------------------------------------------------------------

bool can_read_file( const boost::filesystem::path& path, std::set<ifstream_info::container_type> supported_containers )
{
    if ( auto container = ifstream_container( path ) )
        return supported_containers.find( *container ) != supported_containers.end();

    return false;
}

} // namespace audio
