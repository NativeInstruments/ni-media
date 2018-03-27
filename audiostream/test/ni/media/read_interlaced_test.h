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

#include <ni/media/audio/iotools.h>
#include <ni/media/pcm/iterator.h>
#include <ni/media/pcm/range/converted.h>

#include <ni/media/test_helper.h>

#include <boost/range/algorithm/equal.hpp>
#include <boost/range/iterator_range.hpp>


#include <algorithm>
#include <cmath>
#include <string>
#include <type_traits>
#include <vector>

namespace detail
{

template <class Container, class Stream>
void push_back_samples( Container& container, Stream& stream, size_t num_samples )
{
    Container tmp( num_samples );
    stream >> tmp;
    container.insert( container.end(), tmp.begin(), tmp.end() );
}

} // namespace detail

template <typename Stream>
void read_interlaced_test( Stream&&       stream,
                           size_t         num_frames_per_block,
                           std::streampos frame_pos,
                           std::streamoff frame_offset = 0,
                           size_t         num_blocks   = 1 )
{
    std::vector<float> buffer1, buffer2;

    const auto num_samples = num_frames_per_block * stream.info().num_channels();
    const auto num_bytes   = std::streamoff( num_frames_per_block * stream.info().bytes_per_frame() );

    const auto byte_offset = frame_offset * std::streamoff( stream.info().bytes_per_frame() );

    struct Interval
    {
        std::streampos first, last;
    };

    using Frame = std::array<Interval, 2>;

    Frame expected, actual;

    expected[0].last = frame_pos * stream.info().bytes_per_frame();

    for ( size_t n = 0; n < num_blocks; ++n )
    {
        expected[0].first = expected[0].last;
        actual[0].first   = stream.seekg( expected[0].first ).tellg();

        detail::push_back_samples( buffer1, stream, num_samples );

        expected[0].last = expected[0].first + num_bytes;
        actual[0].last   = stream.tellg();


        expected[1].first = actual[0].first + byte_offset;
        actual[1].first   = stream.seekg( expected[1].first ).tellg();

        detail::push_back_samples( buffer2, stream, num_samples );

        expected[1].last = expected[1].first + num_bytes;
        actual[1].last   = stream.tellg();

        ASSERT_EQ( expected[0].first, actual[0].first ) << "Block Number: " << n;
        ASSERT_EQ( expected[0].last, actual[0].last ) << "Block Number: " << n;
        ASSERT_EQ( expected[1].first, actual[1].first ) << "Block Number: " << n;
        ASSERT_EQ( expected[1].last, actual[1].last ) << "Block Number: " << n;
    }

    if ( frame_offset < 0 )
        std::swap( buffer1, buffer2 );

    const auto offset = std::abs( frame_offset ) * stream.info().num_channels();

    using samples_diff_type = std::iterator_traits<decltype( buffer1.begin() )>::difference_type;
    auto samples1 =
        std::vector<float>( std::next( buffer1.begin(), static_cast<samples_diff_type>( offset ) ), buffer1.end() );

    auto samples2 = std::vector<float>( buffer2.begin(), buffer2.begin() + boost::size( samples1 ) );

    auto float_compare = []( float lhs, float rhs ) {
        return std::abs( lhs - rhs ) <= std::numeric_limits<float>::epsilon();
    };

    bool samples_are_equal = boost::equal( samples1, samples2 );

    EXPECT_TRUE( samples_are_equal );
}
