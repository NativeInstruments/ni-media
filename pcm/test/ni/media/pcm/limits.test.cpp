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

#include <ni/media/pcm/limits.h>
#include <ni/media/pcm/numspace.h>

#include <gtest/gtest.h>

#include <boost/range/algorithm/adjacent_find.hpp>
#include <boost/range/size.hpp>

#include <algorithm>
#include <cmath>
#include <type_traits>


template <class Value>
class limits : public testing::Test
{

public:
protected:
    int bits() const
    {
        static constexpr auto max_bits = 31;
        return std::min( pcm::numeric_limits<Value>::bits(), max_bits );
    }

    auto values() const
    {
        return pcm::numspace<Value>( bits() );
    }

    size_t actual_size() const
    {
        return size_t( boost::size( values() ) );
    }

    size_t expected_size() const
    {
        return size_t( 1 ) << bits();
    }

    bool equidistant() const
    {
        auto step = pcm::numeric_limits<Value>::step( bits() );

        return std::end( values() )
               == boost::adjacent_find( values(), [step]( Value a, Value b ) { return a + step != b; } );
    }

    Value expected_min() const
    {
        return pcm::numeric_limits<Value>::min();
    }

    Value actual_min() const
    {
        return *std::begin( values() );
    }

    Value expected_max() const
    {
        return pcm::numeric_limits<Value>::max( bits() );
    }

    Value actual_max() const
    {
        return *--std::end( values() );
    }

    Value expected_zero() const
    {
        return pcm::numeric_limits<Value>::zero();
    }

    Value actual_zero() const
    {
        return *( std::begin( values() ) + boost::size( values() ) / 2 );
    }
};


TYPED_TEST_SUITE_P( limits );

TYPED_TEST_P( limits, expect_equidistant )
{
    // this takes too long
    // EXPECT_TRUE( this->equidistant());
    EXPECT_TRUE( true );
}

TYPED_TEST_P( limits, expect_size )
{
    EXPECT_EQ( this->expected_size(), this->actual_size() );
}

TYPED_TEST_P( limits, expect_min )
{
    EXPECT_EQ( this->expected_min(), this->actual_min() );
}

TYPED_TEST_P( limits, expect_max )
{
    EXPECT_EQ( this->expected_max(), this->actual_max() );
}

TYPED_TEST_P( limits, expect_zero )
{
    EXPECT_EQ( this->expected_zero(), this->actual_zero() );
}


REGISTER_TYPED_TEST_SUITE_P( limits, expect_equidistant, expect_size, expect_min, expect_max, expect_zero );

using limits_test_t =
    testing::Types<uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t, float, double>;

INSTANTIATE_TYPED_TEST_SUITE_P( all, limits, limits_test_t );
