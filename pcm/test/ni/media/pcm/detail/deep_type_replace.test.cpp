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


#include <ni/media/pcm/detail/deep_type_replace.h>

#include <gtest/gtest.h>

namespace pcm
{
namespace detail
{

// "custom" declval which doesn't add rvalue ref like std::declval does
template <class T>
T deckelval();

template <class T>
auto type_name()
{
    return typeid( deckelval<T> ).name();
}

template <class Expected, class Actual>
struct is_same
{
    static auto constexpr value()
    {
        return ::testing::AssertionFailure()
               << type_name<Expected>() << " does not match " << type_name<Actual>() << std::endl;
    }
};

template <class Expected>
struct is_same<Expected, Expected>
{
    static auto constexpr value()
    {
        return ::testing::AssertionSuccess();
    }
};

template <class... Ts>
struct A;

template <class... Ts>
struct B;

template <class Expected, class Original, class From, class To>
using test = is_same<Expected, deep_type_replace_t<Original, From, To>>;


TEST( pcm_algorithm_detail_dispatch_test, deep_replace )
{

    EXPECT_TRUE( ( test<int, void, void, int>::value() ) );

    EXPECT_TRUE( ( test<int, int, float, int>::value() ) );

    EXPECT_TRUE( ( test<int, int, int, int>::value() ) );

    EXPECT_TRUE( ( test<int, A<>, A<>, int>::value() ) );

    EXPECT_TRUE( ( test<A<>, A<>, int, B<int>>::value() ) );

    EXPECT_TRUE( ( test<A<float>, A<int>, int, float>::value() ) );

    EXPECT_TRUE( ( test<A<int, float&>, A<int, float&>, float, int>::value() ) );

    EXPECT_TRUE( ( test<A<int, int>, A<int, float&&>, float&&, int>::value() ) );

    EXPECT_TRUE( ( test<const A<int, int>, const A<int, float>, float, int>::value() ) );

    EXPECT_TRUE( ( test<const A<int, int>&, const A<int, float>&, float, int>::value() ) );

    EXPECT_TRUE( ( test<A<int, int>&, A<int, const float&>&, const float&, int>::value() ) );

    EXPECT_TRUE( ( test<A<int, const volatile int&>&&, A<int, float>&&, float, const volatile int&>::value() ) );

    EXPECT_TRUE( ( test<A<int, B<volatile B<int, int&&, int>&, int, A<int>&>>,
                        A<int, B<volatile B<int, const float&&, int>&, int, A<int>&>>,
                        const float&&,
                        int&&>::value() ) );

    EXPECT_TRUE( (
        test<A<int, B<float, int, B<float, int, A<int>>>>, A<int, B<float, int, A<int>>>, A<int>, B<float, int, A<int>>>::
            value() ) );
}


} // namespace detail
} // namespace pcm
