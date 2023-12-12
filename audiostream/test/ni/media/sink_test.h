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

#pragma once

#include <ni/media/test_helper.h>

#include <gtest/gtest.h>


//----------------------------------------------------------------------------------------------------------------------

class sink_test : public ::testing::TestWithParam<std::string>
{

public:
    auto input_name() const
    {
        return GetParam();
    }

    auto output_name() const
    {
        auto output_path = test_files_output_path() / boost::filesystem::path( input_name() ).filename();
        return output_path.string();
    }

protected:
    void SetUp() override
    {
        if ( boost::filesystem::exists( output_name() ) )
            boost::filesystem::remove( output_name() );

        ASSERT_FALSE( boost::filesystem::exists( output_name() ) ) << "Could not delete output file";
    }

    void TearDown() override
    {
    }
};
