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


#include <ni/media/pcm/detail/find_unique.h>

#include <gtest/gtest.h>

#include <vector>

namespace pcm
{

namespace detail
{

TEST( pcm_algorithm_detail_find_unique, find_empty )
{
    auto data = std::vector<int>{};
    EXPECT_EQ( 0, std::distance( data.cbegin(), find_unique( data.cbegin(), data.cend(), 5 ) ) );
    EXPECT_EQ( 0, std::distance( data.cbegin(), find_unique( data, 5 ) ) );
}

TEST( pcm_algorithm_detail_find_unique, find_none )
{
    auto data = std::vector<int>{{1, 2, 3, 4}};
    EXPECT_EQ( 4, std::distance( data.cbegin(), find_unique( data.cbegin(), data.cend(), 5 ) ) );
    EXPECT_EQ( 4, std::distance( data.cbegin(), find_unique( data, 5 ) ) );
}

TEST( pcm_algorithm_detail_find_unique, find_single )
{
    auto data = std::vector<int>{{1, 2, 3, 4}};
    EXPECT_EQ( 0, std::distance( data.cbegin(), find_unique( data.cbegin(), data.cend(), 1 ) ) );
    EXPECT_EQ( 0, std::distance( data.cbegin(), find_unique( data, 1 ) ) );
}

TEST( pcm_algorithm_detail_find_unique, find_multiple )
{
    auto data = std::vector<int>{{1, 2, 3, 1}};
    EXPECT_EQ( 4, std::distance( data.cbegin(), find_unique( data.cbegin(), data.cend(), 1 ) ) );
    EXPECT_EQ( 4, std::distance( data.cbegin(), find_unique( data, 1 ) ) );
}

TEST( pcm_algorithm_detail_find_unique_if, find_empty )
{
    auto data = std::vector<int>{};
    EXPECT_EQ( 0, std::distance( data.cbegin(), find_unique_if( data.cbegin(), data.cend(), []( auto x ) {
                                     return x != 0;
                                 } ) ) );
    EXPECT_EQ( 0, std::distance( data.cbegin(), find_unique_if( data, []( auto x ) { return x != 0; } ) ) );
}

TEST( pcm_algorithm_detail_find_unique_if, find_none )
{
    auto data = std::vector<int>{{0, 0, 0, 0}};
    EXPECT_EQ( 4, std::distance( data.cbegin(), find_unique_if( data.cbegin(), data.cend(), []( auto x ) {
                                     return x != 0;
                                 } ) ) );
    EXPECT_EQ( 4, std::distance( data.cbegin(), find_unique_if( data, []( auto x ) { return x != 0; } ) ) );
}

TEST( pcm_algorithm_detail_find_unique_if, find_single )
{
    auto data = std::vector<int>{{1, 0, 0, 0}};
    EXPECT_EQ( 0, std::distance( data.cbegin(), find_unique_if( data.cbegin(), data.cend(), []( auto x ) {
                                     return x != 0;
                                 } ) ) );
    EXPECT_EQ( 0, std::distance( data.cbegin(), find_unique_if( data, []( auto x ) { return x != 0; } ) ) );
}

TEST( pcm_algorithm_detail_find_unique_if, find_multiple )
{
    auto data = std::vector<int>{{0, 1, 3, 0}};
    EXPECT_EQ( 4, std::distance( data.cbegin(), find_unique_if( data.cbegin(), data.cend(), []( auto x ) {
                                     return x != 0;
                                 } ) ) );
    EXPECT_EQ( 4, std::distance( data.cbegin(), find_unique_if( data, []( auto x ) { return x != 0; } ) ) );
}

} // namespace detail
} // namespace pcm
