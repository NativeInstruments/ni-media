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

#include "runtime_format.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <iterator>
#include <type_traits>

#include <boost/algorithm/clamp.hpp>

namespace pcm
{
namespace detail
{

template <typename Source>
Source sign_cast( Source val )
{
    return val;
}

template <typename Target, typename Source>
Target sign_cast( Source val,
                  std::enable_if_t<std::is_signed<Target>::value != std::is_signed<Source>::value>* = nullptr )
{
    static constexpr auto mask = Source{1} << ( sizeof( Source ) * 8 - 1 );

    return static_cast<Target>( val ^ mask );
}

template <typename Integer, typename Real>
Integer round_cast( Real val )
{
    return static_cast<Integer>( val > 0 ? val + Real{0.5} : val - Real{0.5} );
}

template <typename T1, typename T2>
struct is_bigger : public std::integral_constant<bool, ( sizeof( T1 ) > sizeof( T2 ) )>
{
};

template <typename Source>
Source shift_cast( Source val )
{
    return val;
}

template <typename Target, typename Source>
Target shift_cast( Source val, std::enable_if_t<is_bigger<Source, Target>::value>* = nullptr )
{
    return static_cast<Target>( val >> ( 8 * ( sizeof( Source ) - sizeof( Target ) ) ) );
}

template <typename Target, typename Source>
Target shift_cast( Source val, std::enable_if_t<( is_bigger<Target, Source>::value )>* = nullptr )
{
    return static_cast<Target>( val ) << ( 8 * ( sizeof( Target ) - sizeof( Source ) ) );
}

// floating point <- floating point
// static_cast
template <typename Target, typename Source>
Target convert_to(
    Source src,
    std::enable_if_t<std::is_floating_point<Target>::value && std::is_floating_point<Source>::value>* = nullptr )
{
    return static_cast<Target>( src );
}

// fixed point <- floating point
// clamp, upscale, round_cast, sign_cast
template <typename Target, typename Source>
Target convert_to(
    Source src, std::enable_if_t<std::is_integral<Target>::value && std::is_floating_point<Source>::value>* = nullptr )
{
    using SignedTarget = std::make_signed_t<Target>;

    static constexpr auto scale = Source{1ull << ( sizeof( Target ) * 8 - 1 )};
    static constexpr auto min   = Source{-1};
    static constexpr auto max   = Source{1} - std::max( 1 / scale, std::numeric_limits<Source>::epsilon() );

    return sign_cast<Target>( round_cast<SignedTarget>( scale * boost::algorithm::clamp( src, min, max ) ) );
}

// floating point <- fixed point
// sign_cast + downscale
template <typename Target, typename Source>
Target convert_to(
    Source src, std::enable_if_t<std::is_floating_point<Target>::value && std::is_integral<Source>::value>* = nullptr )
{
    using SignedSource = std::make_signed_t<Source>;

    static constexpr auto scale = Target{1} / ( 1ull << ( sizeof( Source ) * 8 - 1 ) );

    return static_cast<Target>( scale * sign_cast<SignedSource>( src ) );
}

// fixed point <- fixed point
// sign_cast, shift_cast
template <typename Target, typename Source>
Target convert_to( Source src,
                   std::enable_if_t<std::is_integral<Target>::value && std::is_integral<Source>::value>* = nullptr )
{
    using SignAdjustedSource = std::conditional_t<std::is_signed<Target>::value, //
                                                  std::make_signed_t<Source>,
                                                  std::make_unsigned_t<Source>>;

    return shift_cast<Target>( sign_cast<SignAdjustedSource>( src ) );
}

template <typename T>
struct storage
{
};

template <endian_type e>
struct storage<::pcm::compiletime_format<::pcm::floating_point, ::pcm::_32bit, e>>
{
    using type = float;
};

template <endian_type e>
struct storage<::pcm::compiletime_format<::pcm::floating_point, ::pcm::_64bit, e>>
{
    using type = double;
};

template <endian_type e>
struct storage<::pcm::compiletime_format<::pcm::signed_integer, ::pcm::_8bit, e>>
{
    using type = int8_t;
};

template <endian_type e>
struct storage<::pcm::compiletime_format<::pcm::signed_integer, ::pcm::_16bit, e>>
{
    using type = int16_t;
};

template <endian_type e>
struct storage<::pcm::compiletime_format<::pcm::signed_integer, ::pcm::_24bit, e>>
{
    using type = int32_t;
};

template <endian_type e>
struct storage<::pcm::compiletime_format<::pcm::signed_integer, ::pcm::_32bit, e>>
{
    using type = int32_t;
};

template <endian_type e>
struct storage<::pcm::compiletime_format<::pcm::signed_integer, ::pcm::_64bit, e>>
{
    using type = int64_t;
};

template <endian_type e>
struct storage<::pcm::compiletime_format<::pcm::unsigned_integer, ::pcm::_8bit, e>>
{
    using type = uint8_t;
};

template <endian_type e>
struct storage<::pcm::compiletime_format<::pcm::unsigned_integer, ::pcm::_16bit, e>>
{
    using type = uint16_t;
};

template <endian_type e>
struct storage<::pcm::compiletime_format<::pcm::unsigned_integer, ::pcm::_24bit, e>>
{
    using type = uint32_t;
};

template <endian_type e>
struct storage<::pcm::compiletime_format<::pcm::unsigned_integer, ::pcm::_32bit, e>>
{
    using type = uint32_t;
};

template <endian_type e>
struct storage<::pcm::compiletime_format<::pcm::unsigned_integer, ::pcm::_64bit, e>>
{
    using type = uint64_t;
};

// equivalent of std::copy_n with the additional garanty that iterators will be incremented exactly n times
template <typename InputIterator, typename Size, typename OutputIterator>
OutputIterator increment_n_copy_n( InputIterator in, Size size, OutputIterator out )
{
    for ( Size i = 0; i != size; ++i, ++in, ++out )
        *out = *in;
    return out;
}

template <typename Format>
struct intermediate
{
    using value_type = typename storage<Format>::type;

    static constexpr auto is_native_endian = Format{}.endian() == native_endian;
    static constexpr auto num_bytes        = Format{}.bitwidth() / 8;
    static constexpr auto padding          = little_endian == native_endian ? sizeof( value_type ) - num_bytes : 0;
    static constexpr auto offset           = is_native_endian ? padding : padding + num_bytes;

    template <typename InputIterator>
    intermediate( InputIterator iter )
    {
        increment_n_copy_n( iter, num_bytes, begin() );
    }

    intermediate( value_type val )
    : m_value( val )
    {
    }

    using iterator       = std::conditional_t<is_native_endian, char*, std::reverse_iterator<char*>>;
    using const_iterator = std::conditional_t<is_native_endian, const char*, std::reverse_iterator<const char*>>;

    auto begin() -> iterator
    {
        return iterator( reinterpret_cast<char*>( &m_value ) + offset );
    }

    auto end() -> iterator
    {
        return begin() + size();
    }

    auto begin() const -> const_iterator
    {
        return const_iterator( reinterpret_cast<char*>( &m_value ) + offset );
    }

    auto end() const -> const_iterator
    {
        return begin() + size();
    }

    auto size() const -> size_t
    {
        return num_bytes;
    }

    auto value() const -> value_type
    {
        return m_value;
    }

private:
    value_type m_value = 0;
};

template <typename Value, typename Iterator, typename Format>
Value read_impl( Iterator iter )
{
    auto data = intermediate<Format>( iter );
    return convert_to<Value>( data.value() );
}

template <typename Value, typename Iterator, typename... Ts>
auto make_read_impls( const std::tuple<Ts...>& ) -> std::array<Value ( * )( Iterator ), sizeof...( Ts )>
{
    return {{&read_impl<Value, Iterator, Ts>...}};
}

template <typename Value, typename Iterator, typename Format>
void write_impl( Iterator iter, Value val )
{
    auto data = intermediate<Format>{convert_to<typename intermediate<Format>::value_type>( val )};
    std::copy( data.begin(), data.end(), iter );
}

template <typename Value, typename Iterator, typename... Ts>
auto make_write_impls( const std::tuple<Ts...>& ) -> std::array<void ( * )( Iterator, Value ), sizeof...( Ts )>
{
    return {{&write_impl<Value, Iterator, Ts>...}};
}

} // namespace detail

template <typename Value, typename Iterator, number_type n, bitwidth_type b, endian_type e>
Value read( Iterator iter, const compiletime_format<n, b, e>& )
{
    return detail::read_impl<Value, Iterator, compiletime_format<n, b, e>>( iter );
}

template <typename Value, typename Iterator>
Value read( Iterator iter, const runtime_format& fmt )
{
    static auto const impls = detail::make_read_impls<Value, Iterator>( compiletime_formats() );
    return impls.at( fmt.index() )( iter );
}

template <typename Value, typename Iterator, number_type n, bitwidth_type b, endian_type e>
void write( Iterator iter, Value val, const compiletime_format<n, b, e>& )
{
    detail::write_impl<Value, Iterator, compiletime_format<n, b, e>>( iter, val );
}

template <typename Value, typename Iterator>
void write( Iterator iter, Value val, const runtime_format& fmt )
{
    static auto const impls = detail::make_write_impls<Value, Iterator>( compiletime_formats() );
    impls.at( fmt.index() )( iter, val );
}


} // namespace pcm
