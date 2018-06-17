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
#include <ni/media/test_helper.h>

#include <boost/preprocessor/stringize.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/tokenizer.hpp>


#include <cstdlib>
#include <iterator>


//--------------------------------------------------------------------------------------------------------------------

namespace
{
std::string get_test_files_path()
{
#ifdef NI_MEDIA_TEST_FILES_PATH
    return BOOST_PP_STRINGIZE( NI_MEDIA_TEST_FILES_PATH );
#else
    const char* test_files_path_env = std::getenv( "NI_MEDIA_TEST_FILES_PATH" );
    return test_files_path_env ? test_files_path_env : "./";
#endif
}

auto supported_files( const boost::filesystem::path&                        root_path,
                      boost::optional<audio::ifstream_info::container_type> container )
{
    using namespace boost::adaptors;
    using namespace boost::filesystem;

    auto can_read_file = [container]( const auto& p ) {
        return container ? audio::can_read_file( p, {*container} ) : audio::can_read_file( p );
    };

    recursive_directory_iterator beg( root_path ), end;
    return boost::make_iterator_range( beg, end ) | transformed( []( const path& p ) { return p.string(); } )
           | filtered( can_read_file );
}
} // namespace

//----------------------------------------------------------------------------------------------------------------------

std::string get_user_files_path()
{
    return get_test_files_path() + "/user_files";
}

//----------------------------------------------------------------------------------------------------------------------

std::string get_reference_files_path()
{
    return get_test_files_path() + "/reference_files";
}

//----------------------------------------------------------------------------------------------------------------------

std::string get_fuzz_files_path()
{
    return get_test_files_path() + "/fuzz_files";
}

//----------------------------------------------------------------------------------------------------------------------

std::string get_output_files_path()
{
#ifdef NI_MEDIA_OUTPUT_FILES_PATH
    return BOOST_PP_STRINGIZE( NI_MEDIA_OUTPUT_FILES_PATH );
#else
    const char* output_files_path_env = std::getenv( "NI_MEDIA_OUTPUT_FILES_PATH" );
    return output_files_path_env ? output_files_path_env : "./";
#endif
}

//----------------------------------------------------------------------------------------------------------------------

std::vector<std::string> collect_supported_files( const boost::filesystem::path&                        root_path,
                                                  boost::optional<audio::ifstream_info::container_type> container )
{
    using Files = std::vector<std::string>;
    return boost::copy_range<Files>( supported_files( root_path, container ) );
}
