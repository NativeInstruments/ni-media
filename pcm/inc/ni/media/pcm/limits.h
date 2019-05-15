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

#pragma once

#include <cstdint>
#include <limits>
#include <type_traits>

namespace pcm
{

namespace detail
{

template <class T>
using enable_if_float_t = std::enable_if_t<std::is_floating_point<T>::value>;

template <class T>
using enable_if_integral_t = std::enable_if_t<std::is_integral<T>::value>;

template <class T>
auto constexpr bits( enable_if_integral_t<T>* = 0 )
{
    return int( sizeof( T ) * 8 );
}

template <class T>
auto constexpr bits( enable_if_float_t<T>* = 0 )
{
    return int( std::numeric_limits<T>::digits );
}

template <class T>
auto constexpr step( int bits_, enable_if_integral_t<T>* = 0 )
{
    return std::make_unsigned_t<T>( 1 ) << ( bits<T>() - bits_ );
}

template <class T>
auto constexpr step( int bits_, enable_if_float_t<T>* = 0 )
{
    return T( 2 ) / ( 1ull << bits_ );
}

template <class T>
auto constexpr scale()
{
    return step<T>( 1 );
}

template <class T>
auto constexpr min( enable_if_integral_t<T>* = 0 )
{
    return T( std::numeric_limits<T>::min() );
}

template <class T>
auto constexpr min( enable_if_float_t<T>* = 0 )
{
    return T( -1 );
}

template <class T>
auto constexpr zero( enable_if_integral_t<T>* = 0 )
{
    return T( min<T>() + scale<T>() );
}

template <class T>
auto constexpr zero( enable_if_float_t<T>* = 0 )
{
    return T( 0 );
}

template <class T>
auto constexpr max( int bits_, enable_if_integral_t<T>* = 0 )
{
    return T( std::numeric_limits<T>::max() - step<T>( bits_ ) + 1 );
}

template <class T>
auto constexpr max( int bits_, enable_if_float_t<T>* = 0 )
{
    return T( 1 - step<T>( bits_ ) );
}

} // namespace detail

template <class T>
struct numeric_limits
{
    static_assert( std::is_arithmetic<T>::value, "Must be arithmetic type" );

    static constexpr int bits()
    {
        return detail::bits<T>();
    }

    static constexpr auto step( int bits_ = bits() )
    {
        return detail::step<T>( bits_ );
    }

    static constexpr T min()
    {
        return detail::min<T>();
    }

    static constexpr T max( int bits_ = bits() )
    {
        return detail::max<T>( bits_ );
    }

    static constexpr T zero()
    {
        return detail::zero<T>();
    }

    static constexpr T scale()
    {
        return detail::scale<T>();
    }
};


} // namespace pcm
