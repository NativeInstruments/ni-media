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

#include <ni/media/pcm/algorithm.h>
#include <ni/media/pcm/iterator.h>

#include <gtest/gtest.h>


TEST( dispatch_test, dispatch_throws_runtime_error_on_imcompatible_formats )
{
    auto in  = std::vector<char>();
    auto out = std::vector<float>();

    EXPECT_THROW(
        ( pcm::copy( pcm::make_iterator<float>( in.begin(), pcm::runtime_format( pcm::signed_integer, pcm::_24bit ) ),
                     pcm::make_iterator<float>( in.end(), pcm::runtime_format( pcm::signed_integer, pcm::_16bit ) ),
                     out.begin() ) ),
        std::runtime_error );
}
