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

#include <ni/media/pcm/detail/tuple_to_array.h>

#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>


namespace pcm
{

namespace detail
{
//----------------------------------------------------------------------------------------------------------------------

TEST( tuple_to_array_test, empty )
{
    auto arr = tuple_to_array<int>( std::tuple<>{} );

    EXPECT_TRUE( ( std::is_same<std::array<int, 0>, decltype( arr )>::value ) );
    EXPECT_EQ( 0u, arr.size() );
}

//----------------------------------------------------------------------------------------------------------------------

TEST( tuple_to_array_test, no_conversions )
{
    auto tup = std::make_tuple( 1, 2, 3 );
    auto arr = tuple_to_array<int>( tup );
    auto exp = std::array<int, 3>{{1, 2, 3}};

    EXPECT_TRUE( ( std::is_same<decltype( exp ), decltype( arr )>::value ) );
    EXPECT_EQ( exp, arr );
}

//----------------------------------------------------------------------------------------------------------------------

TEST( tuple_to_array_test, implicit_conversions )
{
    auto tup = std::make_tuple( 1u, 2, 3.f, 4.0 );
    auto arr = tuple_to_array<double>( tup );
    auto exp = std::array<double, 4>{{1.0, 2.0, 3.0, 4.0}};

    EXPECT_TRUE( ( std::is_same<decltype( exp ), decltype( arr )>::value ) );
    EXPECT_EQ( exp, arr );
}

//----------------------------------------------------------------------------------------------------------------------

TEST( tuple_to_array_test, narrowing_conversions )
{
    auto tup = std::make_tuple( 1u, 2, 3.f, 4.0 );
    auto arr = tuple_to_array<int>( tup );
    auto exp = std::array<int, 4>{{1, 2, 3, 4}};

    EXPECT_TRUE( ( std::is_same<decltype( exp ), decltype( arr )>::value ) );
    EXPECT_EQ( exp, arr );
}

//----------------------------------------------------------------------------------------------------------------------

TEST( tuple_to_array_test, std_pair )
{
    auto tup = std::make_pair( 1, 2.f );
    auto arr = tuple_to_array<int>( tup );
    auto exp = std::array<int, 2>{{1, 2}};

    EXPECT_TRUE( ( std::is_same<decltype( exp ), decltype( arr )>::value ) );
    EXPECT_EQ( exp, arr );
}

//----------------------------------------------------------------------------------------------------------------------

namespace
{

// clang-format off
template <typename T>
struct traits{};

template <> struct traits<int>          : std::integral_constant<char, 'i'>{};
template <> struct traits<float>        : std::integral_constant<char, 'f'>{};
template <> struct traits<double>       : std::integral_constant<char, 'd'>{};
template <> struct traits<char>         : std::integral_constant<char, 'c'>{};
template <> struct traits<const char*>  : std::integral_constant<char, 'l'>{};
// clang-format on

std::string i_like_to_move_it;

class foo
{
public:
    foo()             = default;
    foo( const foo& ) = default;

    template <typename T>
    foo( const T& t )
    {
        std::stringstream stream;
        stream << t << traits<T>::value;
        m_str = stream.str();
    }

    foo( foo&& f )
    : m_str( std::move( f.m_str ) )
    {
        i_like_to_move_it += m_str;
    }

    operator std::string() const
    {
        return m_str;
    }

private:
    std::string m_str;
};

} // namespace

TEST( tuple_to_array_test, types_match )
{
    i_like_to_move_it.clear();
    auto tup = std::make_tuple( 1, 2.f, 3.0, '4', "5" );
    auto arr = tuple_to_array<foo>( tup );
    auto vec = std::vector<std::string>( arr.begin(), arr.end() );
    auto exp = std::vector<std::string>{{"1i", "2f", "3d", "4c", "5l"}};

    EXPECT_TRUE( ( std::is_same<std::array<foo, 5>, decltype( arr )>::value ) );
    EXPECT_EQ( exp, vec );
    EXPECT_EQ( std::string(), i_like_to_move_it );
}

TEST( tuple_to_array_test, elements_are_moved_when_tuple_is_rvalue_ref )
{
    foo foo_1{1}, foo_2{2.f}, foo_4{'4'};

    i_like_to_move_it.clear();
    auto arr = tuple_to_array<foo>( std::make_tuple( foo_1, foo_2, 3.0, foo_4, "5" ) );
    auto vec = std::vector<std::string>( arr.begin(), arr.end() );
    auto exp = std::vector<std::string>{{"1i", "2f", "3d", "4c", "5l"}};

    EXPECT_TRUE( ( std::is_same<std::array<foo, 5>, decltype( arr )>::value ) );
    EXPECT_EQ( exp, vec );
    EXPECT_EQ( std::string( "1i2f4c" ), i_like_to_move_it );
}


} // namespace detail
} // namespace pcm
