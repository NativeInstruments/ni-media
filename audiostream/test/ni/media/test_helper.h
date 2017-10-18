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

#pragma once

#include <ni/media/audio/ifstream_info.h>

#include <gtest/gtest.h>

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>

#include <string>
#include <vector>

//----------------------------------------------------------------------------------------------------------------------

std::string get_reference_files_path();
std::string get_user_files_path();
std::string get_output_files_path();

std::vector<std::string> collect_supported_files( const boost::filesystem::path&,
                                                  boost::optional<audio::ifstream_info::container_type> container );

//----------------------------------------------------------------------------------------------------------------------

inline auto user_files( boost::optional<audio::ifstream_info::container_type> container = boost::none )
{
    return ::testing::ValuesIn( collect_supported_files( get_user_files_path(), container ) );
}

inline auto reference_files( boost::optional<audio::ifstream_info::container_type> container = boost::none )
{
    return ::testing::ValuesIn( collect_supported_files( get_reference_files_path(), container ) );
}
