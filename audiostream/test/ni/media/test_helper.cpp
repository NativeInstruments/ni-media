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

#include <ni/media/audio/iotools.h>
#include <ni/media/test_helper.h>

#include <boost/locale.hpp>
#include <boost/predef/os.h>
#include <boost/preprocessor/stringize.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/tokenizer.hpp>

#include <codecvt>
#include <cstdlib>
#include <iterator>

//--------------------------------------------------------------------------------------------------------------------

namespace
{

bool init()
{
#if BOOST_OS_WINDOWS
    boost::filesystem::path::imbue( std::locale( std::locale(), new std::codecvt_utf8_utf16<wchar_t>() ) );
#endif
    return true;
}

const bool initialized = init();

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

//----------------------------------------------------------------------------------------------------------------------

std::vector<std::string> collect_supported_files( const boost::filesystem::path&                        root_path,
                                                  boost::optional<audio::ifstream_info::container_type> container )
{
    using Files = std::vector<std::string>;
    return boost::copy_range<Files>( supported_files( root_path, container ) );
}

} // namespace


//----------------------------------------------------------------------------------------------------------------------

boost::filesystem::path test_files_input_path()
{
    return {BOOST_PP_STRINGIZE( NI_MEDIA_TEST_FILES_PATH )};
}

//----------------------------------------------------------------------------------------------------------------------

boost::filesystem::path test_files_output_path()
{
    return {BOOST_PP_STRINGIZE( NI_MEDIA_OUTPUT_FILES_PATH )};
}

//----------------------------------------------------------------------------------------------------------------------


TestFiles user_files( boost::optional<audio::ifstream_info::container_type> container )
{
    return ::testing::ValuesIn( collect_supported_files( test_files_input_path() / "user_files", container ) );
}

//----------------------------------------------------------------------------------------------------------------------

TestFiles reference_files( boost::optional<audio::ifstream_info::container_type> container )
{
    return ::testing::ValuesIn( collect_supported_files( test_files_input_path() / "reference_files", container ) );
}

//----------------------------------------------------------------------------------------------------------------------


TestFiles fuzz_files( boost::optional<audio::ifstream_info::container_type> container )
{
    return ::testing::ValuesIn( collect_supported_files( test_files_input_path() / "fuzz_files", container ) );
}

//----------------------------------------------------------------------------------------------------------------------
