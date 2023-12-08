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

#include <ni/media/pcm/converter.h>
#include <ni/media/pcm/numspace.h>

#include <gtest/gtest.h>

using all_integral_types       = std::tuple<uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t>;
using all_floating_point_types = std::tuple<float, double>;
using all_pcm_types            = decltype( std::tuple_cat( all_integral_types{}, all_floating_point_types{} ) );


template <class Source, class Target>
struct from_to
{
    using source_t = Source;
    using target_t = Target;
};

template <class Source, class Target>
struct make_from_to;

template <class... Sources, class Target>
struct make_from_to<std::tuple<Sources...>, Target>
{
    using type = testing::Types<from_to<Sources, Target>...>;
};

template <class Source, class... Targets>
struct make_from_to<Source, std::tuple<Targets...>>
{
    using type = testing::Types<from_to<Source, Targets>...>;
};

template <class From>
struct from
{
    template <class To>
    using to = typename make_from_to<From, To>::type;
};


template <class Traits>
class pcm_convert_test : public testing::Test
{
public:
    using source_t = typename Traits::source_t;
    using target_t = typename Traits::target_t;

    auto bits() const
    {
        return std::min( pcm::numeric_limits<source_t>::bits(), pcm::numeric_limits<target_t>::bits() );
    }
};


TYPED_TEST_SUITE_P( pcm_convert_test );

TYPED_TEST_P( pcm_convert_test, convert_whole_range )
{
    using source_t = typename TestFixture::source_t;
    using target_t = typename TestFixture::target_t;

    // we limit the precision to 24 bit: good compromise between test accuracy and overall test run time
    auto bits = std::min( this->bits(), 24 );

    auto expected = pcm::numspace<target_t>( bits );
    auto actual   = pcm::numspace<source_t>( bits )
                  | boost::adaptors::transformed( []( auto v ) { return pcm::detail::convert_to<target_t>( v ); } );

    EXPECT_TRUE( boost::equal( expected, actual ) );
}


REGISTER_TYPED_TEST_SUITE_P( pcm_convert_test, convert_whole_range );

INSTANTIATE_TYPED_TEST_SUITE_P( uint8_to_all_pcm_types, pcm_convert_test, from<uint8_t>::to<all_pcm_types> );
INSTANTIATE_TYPED_TEST_SUITE_P( uint16_to_all_pcm_types, pcm_convert_test, from<uint16_t>::to<all_pcm_types> );
INSTANTIATE_TYPED_TEST_SUITE_P( uint32_to_all_pcm_types, pcm_convert_test, from<uint32_t>::to<all_pcm_types> );
INSTANTIATE_TYPED_TEST_SUITE_P( uint64_to_all_pcm_types, pcm_convert_test, from<uint64_t>::to<all_pcm_types> );
INSTANTIATE_TYPED_TEST_SUITE_P( int8_to_all_pcm_types, pcm_convert_test, from<int8_t>::to<all_pcm_types> );
INSTANTIATE_TYPED_TEST_SUITE_P( int16_to_all_pcm_types, pcm_convert_test, from<int16_t>::to<all_pcm_types> );
INSTANTIATE_TYPED_TEST_SUITE_P( int32_to_all_pcm_types, pcm_convert_test, from<int32_t>::to<all_pcm_types> );
INSTANTIATE_TYPED_TEST_SUITE_P( int64_to_all_pcm_types, pcm_convert_test, from<int64_t>::to<all_pcm_types> );
INSTANTIATE_TYPED_TEST_SUITE_P( float_to_all_pcm_types, pcm_convert_test, from<float>::to<all_pcm_types> );
INSTANTIATE_TYPED_TEST_SUITE_P( double_to_all_pcm_types, pcm_convert_test, from<double>::to<all_pcm_types> );

//----------------------------------------------------------------------------------------------------------------------

template <class Traits>
class pcm_clip_test : public pcm_convert_test<Traits>
{
};

TYPED_TEST_SUITE_P( pcm_clip_test );

TYPED_TEST_P( pcm_clip_test, clip_to_max )
{
    using source_t = typename TestFixture::source_t;
    using target_t = typename TestFixture::target_t;

    auto expected_max = *--boost::end( pcm::numspace<target_t>( this->bits() ) );
    auto actual_max   = pcm::detail::convert_to<target_t>( std::numeric_limits<source_t>::max() );

    EXPECT_TRUE( ( std::is_same<decltype( actual_max ), decltype( expected_max )>::value ) );
    EXPECT_GE( actual_max, expected_max );
}

TYPED_TEST_P( pcm_clip_test, clip_to_min )
{
    using source_t = typename TestFixture::source_t;
    using target_t = typename TestFixture::target_t;

    auto expected_min = *boost::begin( pcm::numspace<target_t>( this->bits() ) );
    auto actual_min   = pcm::detail::convert_to<target_t>( std::numeric_limits<source_t>::lowest() );

    EXPECT_TRUE( ( std::is_same<decltype( actual_min ), decltype( expected_min )>::value ) );
    EXPECT_EQ( actual_min, expected_min );
}

REGISTER_TYPED_TEST_SUITE_P( pcm_clip_test, clip_to_max, clip_to_min );

INSTANTIATE_TYPED_TEST_SUITE_P( uint8_to_all_integral_types, pcm_clip_test, from<uint8_t>::to<all_integral_types> );
INSTANTIATE_TYPED_TEST_SUITE_P( uint16_to_all_integral_types, pcm_clip_test, from<uint16_t>::to<all_integral_types> );
INSTANTIATE_TYPED_TEST_SUITE_P( uint32_to_all_integral_types, pcm_clip_test, from<uint32_t>::to<all_integral_types> );
INSTANTIATE_TYPED_TEST_SUITE_P( uint64_to_all_integral_types, pcm_clip_test, from<uint64_t>::to<all_integral_types> );
INSTANTIATE_TYPED_TEST_SUITE_P( int8_to_all_integral_types, pcm_clip_test, from<int8_t>::to<all_integral_types> );
INSTANTIATE_TYPED_TEST_SUITE_P( int16_to_all_integral_types, pcm_clip_test, from<int16_t>::to<all_integral_types> );
INSTANTIATE_TYPED_TEST_SUITE_P( int32_to_all_integral_types, pcm_clip_test, from<int32_t>::to<all_integral_types> );
INSTANTIATE_TYPED_TEST_SUITE_P( int64_to_all_integral_types, pcm_clip_test, from<int64_t>::to<all_integral_types> );
INSTANTIATE_TYPED_TEST_SUITE_P( float_to_all_integral_types, pcm_clip_test, from<float>::to<all_integral_types> );
INSTANTIATE_TYPED_TEST_SUITE_P( double_to_all_integral_types, pcm_clip_test, from<double>::to<all_integral_types> );
