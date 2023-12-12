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

#include <gtest/gtest.h>

#include <ni/media/audio/ifstream.h>
#include <ni/media/source_test.h>
#include <ni/media/test_helper.h>

#include <vector>

//----------------------------------------------------------------------------------------------------------------------

class ifstream_robustness_test : public source_test
{
};

//----------------------------------------------------------------------------------------------------------------------

TEST_P( ifstream_robustness_test, stream_reaches_eof_on_first_read )
{
    auto stream = open_file_as<audio::ifstream>();

    if ( !stream )
        return;

    auto buffer = std::vector<float>( 256 );
    stream >> buffer;

    EXPECT_TRUE( stream.eof() );
}


INSTANTIATE_TEST_SUITE_P( fuzz_files, ifstream_robustness_test, fuzz_files(), ParamToString{} );
