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

#pragma once

#include <ni/media/audio/ifstream_info.h>

#include <gtest/gtest.h>

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>

#include <cctype>
#include <string>
#include <vector>

using TestFiles = decltype( ::testing::ValuesIn( std::declval<std::vector<std::string>>() ) );

boost::filesystem::path test_files_input_path();
boost::filesystem::path test_files_output_path();

TestFiles user_files( boost::optional<audio::ifstream_info::container_type> container = boost::none );
TestFiles reference_files( boost::optional<audio::ifstream_info::container_type> container = boost::none );
TestFiles fuzz_files( boost::optional<audio::ifstream_info::container_type> container = boost::none );


struct ParamToString
{
    template <class T>
    std::string operator()( const T& info )
    {
        auto filename = boost::filesystem::path( info.param ).filename().string();
        std::replace_if(
            filename.begin(), filename.end(), []( unsigned char c ) { return !std::isalnum( c ); }, '_' );
        return std::to_string( info.index ) + "_" + filename;
    }
};
