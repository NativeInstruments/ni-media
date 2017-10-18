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

#include <ni/media/pcm/detail/tuple_find.h>

#include <boost/detail/endian.hpp> // BOOST_LITTLE_ENDIAN
#include <boost/range/algorithm/find_if.hpp>

#include <array>
#include <stdexcept>
#include <type_traits>


namespace pcm
{

enum number
{
    signed_integer = 0,
    unsigned_integer,
    floating_point
};

enum bitwidth
{
    _8bit  = 8,
    _16bit = 16,
    _24bit = 24,
    _32bit = 32,
    _64bit = 64
};

enum endian
{
    big_endian = 0,
    little_endian,
#if defined( BOOST_BIG_ENDIAN )
    native_endian = big_endian,
#elif defined( BOOST_LITTLE_ENDIAN )
    native_endian = little_endian,
#else
#error "unable to determine system endianness."
#endif
};

struct format
{
    template <number n, bitwidth b, endian e>
    struct tag;

    using tags = std::tuple<tag<signed_integer, _8bit, big_endian>,
                            tag<signed_integer, _8bit, little_endian>,
                            tag<signed_integer, _16bit, big_endian>,
                            tag<signed_integer, _16bit, little_endian>,
                            tag<signed_integer, _24bit, big_endian>,
                            tag<signed_integer, _24bit, little_endian>,
                            tag<signed_integer, _32bit, big_endian>,
                            tag<signed_integer, _32bit, little_endian>,
                            tag<signed_integer, _64bit, big_endian>,
                            tag<signed_integer, _64bit, little_endian>,
                            tag<unsigned_integer, _8bit, big_endian>,
                            tag<unsigned_integer, _8bit, little_endian>,
                            tag<unsigned_integer, _16bit, big_endian>,
                            tag<unsigned_integer, _16bit, little_endian>,
                            tag<unsigned_integer, _24bit, big_endian>,
                            tag<unsigned_integer, _24bit, little_endian>,
                            tag<unsigned_integer, _32bit, big_endian>,
                            tag<unsigned_integer, _32bit, little_endian>,
                            tag<unsigned_integer, _64bit, big_endian>,
                            tag<unsigned_integer, _64bit, little_endian>,
                            tag<floating_point, _32bit, big_endian>,
                            tag<floating_point, _32bit, little_endian>,
                            tag<floating_point, _64bit, big_endian>,
                            tag<floating_point, _64bit, little_endian>>;

    template <class Format>
    using is_supported =
        std::integral_constant<bool, detail::tuple_find<Format, tags>::value != std::tuple_size<tags>::value>;

    template <number n = signed_integer, bitwidth b = _8bit, endian e = native_endian>
    struct tag
    {
        static_assert( is_supported<tag>::value, "Unsupported format" );

        constexpr tag()   = default;
        tag( const tag& ) = default;

        tag( const format& )
        {
        }

        operator number() const
        {
            return n;
        }
        operator bitwidth() const
        {
            return b;
        }
        operator endian() const
        {
            return e;
        }
    };

    format( const format& ) = default;

    template <class Format = tag<>, class = std::enable_if_t<is_supported<Format>::value>>
    constexpr format( const Format& fmt = {} )
    : m_index( get_index( fmt ) )
    {
    }

    format( number n, bitwidth b, endian e = native_endian );

    format( number n, size_t bits, endian e = native_endian );

    format( bool is_signed, bool is_integer, size_t bits, endian e = native_endian );

    friend bool operator==( const format& lhs, const format& rhs );
    friend bool operator!=( const format& lhs, const format& rhs );

    friend size_t get_index( const format& f );
    friend bool is_valid( const format& f );

private:
    static const size_t end_index = std::tuple_size<format::tags>::value;

    size_t m_index = end_index;
};

inline bool operator==( const format& lhs, const format& rhs )
{
    return ( lhs.m_index == rhs.m_index );
}
inline bool operator!=( const format& lhs, const format& rhs )
{
    return ( lhs.m_index != rhs.m_index );
}

auto formats() -> const std::array<format, std::tuple_size<format::tags>::value>&;

template <number n, bitwidth b, endian e>
constexpr auto get_number( const format::tag<n, b, e>& ) -> number
{
    return n;
}
auto get_number( const format& f ) -> number;

template <number n, bitwidth b, endian e>
constexpr auto get_bitwidth( const format::tag<n, b, e>& ) -> bitwidth
{
    return b;
}
auto get_bitwidth( const format& f ) -> bitwidth;

template <number n, bitwidth b, endian e>
constexpr auto get_endian( const format::tag<n, b, e>& ) -> endian
{
    return e;
}
auto get_endian( const format& f ) -> endian;

template <number n, bitwidth b, endian e>
constexpr auto get_index( const format::tag<n, b, e>& ) -> size_t
{
    return detail::tuple_find<format::tag<n, b, e>, format::tags>::value;
}
auto get_index( const format& f ) -> size_t;

template <number n, bitwidth b, endian e>
constexpr auto is_valid( const format::tag<n, b, e>& f ) -> size_t
{
    return get_index( f ) != formats().size();
}
auto is_valid( const format& f ) -> bool;

template <number n, bitwidth b, endian e = native_endian>
constexpr auto make_format( const format::tag<n, b, e>& = {} ) -> format
{
    return format::tag<n, b, e>{};
}
auto make_format( number n, bitwidth b, endian e = native_endian ) -> format;


} // namespace pcm
