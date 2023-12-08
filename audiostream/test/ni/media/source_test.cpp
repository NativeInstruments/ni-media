//
// Copyright (c) 2017-2023 Native Instruments GmbH, Berlin
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

#include "source_test.h"

#include <ni/media/audio/iotools.h>

#include <fstream>

namespace detail
{

template <class Container>
Container read_file_to( std::string file_name, std::ios::openmode mode = std::ios::in | std::ios::binary )
{
    std::filebuf fb;
    fb.open( file_name, mode );

    auto size = fb.pubseekoff( 0, std::ios::end );
    fb.pubseekoff( 0, std::ios::beg ); // rewind

    Container container( static_cast<typename Container::size_type>( size ) );
    fb.sgetn( container.data(), container.size() );
    return container;
}

//----------------------------------------------------------------------------------------------------------------------

template <class Stream>
struct stream_opener;

//----------------------------------------------------------------------------------------------------------------------

template <>
struct stream_opener<audio::ifstream>
{
    static auto open( const std::string& file_name ) -> audio::ifstream
    {
        return {file_name};
    }
};

//----------------------------------------------------------------------------------------------------------------------


template <>
struct stream_opener<audio::ivectorstream>
{
    static auto open( const std::string& file_name ) -> audio::ivectorstream
    {
        return {stream_opener<audio::ifstream>::open( file_name )};
    }
};

//----------------------------------------------------------------------------------------------------------------------

template <>
struct stream_opener<audio::ifvectorstream>
{
    static auto open( const std::string& file_name ) -> audio::ifvectorstream
    {
        auto vec       = read_file_to<std::vector<char>>( file_name );
        auto container = audio::ifstream_container( file_name );
        if ( !container )
            throw std::runtime_error( "Unsupported container_type" );

        return {vec, *container};
    }
};

//----------------------------------------------------------------------------------------------------------------------

template <class Stream>
void open_file_impl( Stream& stream, const std::string& filename )
{

    std::string error;
    try
    {
        stream = detail::stream_opener<Stream>::open( filename );
    }
    catch ( const std::runtime_error& re )
    {
        error = std::string( "Runtime error: " ) + re.what();
    }
    catch ( const std::exception& ex )
    {
        error = std::string( "Unknown error: " ) + ex.what();
    }
    catch ( ... )
    {
        error = std::string( "Unknown error" );
    }

    ASSERT_TRUE( stream.good() ) << error << "\nUnable to open file: " << filename;
}

} // namespace detail


template <class Stream>
Stream source_test::open_file_as()
{
    Stream stream;
    detail::open_file_impl( stream, file_name() );
    return stream;
}

template audio::ifstream       source_test::open_file_as<audio::ifstream>();
template audio::ivectorstream  source_test::open_file_as<audio::ivectorstream>();
template audio::ifvectorstream source_test::open_file_as<audio::ifvectorstream>();
