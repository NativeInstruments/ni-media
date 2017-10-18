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

#include <ni/media/pcm/converter.h>
#include <ni/media/pcm/numspace.h>

#include <gtest/gtest.h>


// We define a test fixture class template.
template <class Traits>
class PcmConverterTest : public testing::Test
{
    using source_t = typename Traits::source_t;
    using target_t = typename Traits::target_t;

protected:
    bool testConversion() const
    {
        int bits = 8;

        auto src = pcm::numspace<source_t>( bits );
        auto trg = pcm::numspace<target_t>( bits );
        auto tst =
            src | boost::adaptors::transformed( []( auto v ) { return pcm::detail::convert_to<target_t>( v ); } );

        return boost::equal( trg, tst );
    }
};


TYPED_TEST_CASE_P( PcmConverterTest );

TYPED_TEST_P( PcmConverterTest, ConversionEqualty )
{
    EXPECT_TRUE( this->testConversion() );
}

REGISTER_TYPED_TEST_CASE_P( PcmConverterTest, ConversionEqualty );

using all_t = std::tuple<uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t, float, double>;

template <class Source, class Target>
struct from_to
{
    using source_t = Source;
    using target_t = Target;
};

template <class Source, class Target>
struct make_converter_test
{
};

template <class... Sources, class Target>
struct make_converter_test<std::tuple<Sources...>, Target>
{
    using type = testing::Types<from_to<Sources, Target>...>;
};

template <class Source, class... Targets>
struct make_converter_test<Source, std::tuple<Targets...>>
{
    using type = testing::Types<from_to<Source, Targets>...>;
};

template <class Source>
using from_t = typename make_converter_test<Source, all_t>::type;

template <class Target>
using to_t = typename make_converter_test<all_t, Target>::type;


INSTANTIATE_TYPED_TEST_CASE_P( Uint8ToAll, PcmConverterTest, from_t<uint8_t> );
INSTANTIATE_TYPED_TEST_CASE_P( Uint16ToAll, PcmConverterTest, from_t<uint16_t> );
INSTANTIATE_TYPED_TEST_CASE_P( Uint32ToAll, PcmConverterTest, from_t<uint32_t> );
INSTANTIATE_TYPED_TEST_CASE_P( Uint64ToAll, PcmConverterTest, from_t<uint64_t> );
INSTANTIATE_TYPED_TEST_CASE_P( Int8ToAll, PcmConverterTest, from_t<int8_t> );
INSTANTIATE_TYPED_TEST_CASE_P( Int16ToAll, PcmConverterTest, from_t<int16_t> );
INSTANTIATE_TYPED_TEST_CASE_P( Int32ToAll, PcmConverterTest, from_t<int32_t> );
INSTANTIATE_TYPED_TEST_CASE_P( Int64ToAll, PcmConverterTest, from_t<int64_t> );
INSTANTIATE_TYPED_TEST_CASE_P( FloatToAll, PcmConverterTest, from_t<float> );
INSTANTIATE_TYPED_TEST_CASE_P( DoubleToAll, PcmConverterTest, from_t<double> );

INSTANTIATE_TYPED_TEST_CASE_P( AllToUint8, PcmConverterTest, to_t<uint8_t> );
INSTANTIATE_TYPED_TEST_CASE_P( AllToUint16, PcmConverterTest, to_t<uint16_t> );
INSTANTIATE_TYPED_TEST_CASE_P( AllToUint32, PcmConverterTest, to_t<uint32_t> );
INSTANTIATE_TYPED_TEST_CASE_P( AllToUint64, PcmConverterTest, to_t<uint64_t> );
INSTANTIATE_TYPED_TEST_CASE_P( AllToInt8, PcmConverterTest, to_t<int8_t> );
INSTANTIATE_TYPED_TEST_CASE_P( AllToInt16, PcmConverterTest, to_t<int16_t> );
INSTANTIATE_TYPED_TEST_CASE_P( AllToInt32, PcmConverterTest, to_t<int32_t> );
INSTANTIATE_TYPED_TEST_CASE_P( AllToInt64, PcmConverterTest, to_t<int64_t> );
INSTANTIATE_TYPED_TEST_CASE_P( AllToFloat, PcmConverterTest, to_t<float> );
INSTANTIATE_TYPED_TEST_CASE_P( AllToDouble, PcmConverterTest, to_t<double> );
