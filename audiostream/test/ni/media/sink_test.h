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

#include <ni/media/test_helper.h>

#include <gtest/gtest.h>

#include <filesystem>

//----------------------------------------------------------------------------------------------------------------------

class sink_test : public ::testing::TestWithParam<std::filesystem::path>
{

public:
    auto input_name() const
    {
        return GetParam();
    }

    auto output_name() const
    {
        return test_files_output_path() / (input_name().filename());
    }

protected:
    void SetUp() override
    {
        if ( std::filesystem::exists( output_name() ) )
            std::filesystem::remove( output_name() );

        ASSERT_FALSE( std::filesystem::exists( output_name() ) ) << "Could not delete output file";
    }

    void TearDown() override
    {
    }
};
