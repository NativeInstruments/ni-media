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

#include "format.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <iterator>
#include <type_traits>


namespace pcm
{
namespace detail
{

template <class Real, size_t Bits>
struct real_promotion
{
    using type = typename std::conditional<( 23 < Bits ), double, float>::type;
};

template <size_t Bits>
struct real_promotion<double, Bits>
{
    using type = double;
};


template <typename Target, typename Source>
Target sign_cast( Source val )
{
    const bool toggle_msb = std::is_signed<Target>::value != std::is_signed<Source>::value;
    const auto mask       = Source{1} << ( sizeof( Source ) * 8 - 1 );

    return static_cast<Target>( toggle_msb ? val ^ mask : val );
}

template <typename Integer, typename Real>
Integer round_cast( Real val )
{
    return static_cast<Integer>( val > 0 ? val + Real{0.5} : val - Real{0.5} );
}

template <class T1, class T2>
struct is_bigger : public std::integral_constant<bool, ( sizeof( T1 ) > sizeof( T2 ) )>
{
};

template <typename Source>
Source shift_cast( Source val )
{
    return val;
}

template <typename Target, typename Source>
typename std::enable_if<is_bigger<Source, Target>::value, Target>::type shift_cast( Source val )
{
    return static_cast<Target>( val >> ( 8 * ( sizeof( Source ) - sizeof( Target ) ) ) );
}

template <typename Target, typename Source>
typename std::enable_if<( is_bigger<Target, Source>::value ), Target>::type shift_cast( Source val )
{
    return static_cast<Target>( val ) << ( 8 * ( sizeof( Target ) - sizeof( Source ) ) );
}


// floating point -> floating point
// simple cast
template <typename Target, typename Source>
typename std::enable_if<std::is_floating_point<Target>::value && std::is_floating_point<Source>::value, Target>::type
convert_to( Source src )
{
    return static_cast<Target>( src );
}

// floating point -> fixed point
// clip, upscale, round_cast, sign_cast
template <typename Target, typename Source>
typename std::enable_if<std::is_integral<Target>::value && std::is_floating_point<Source>::value, Target>::type
convert_to( Source src )
{
    using SignedTarget = typename std::make_signed<Target>::type;
    using Real         = typename real_promotion<Source, sizeof( Target ) * 8>::type;

    const auto scaleValue = static_cast<Real>( 1ull << ( sizeof( Target ) * 8 - 1 ) );
    const auto minValue   = static_cast<Real>( -1.0 );
    const auto maxValue   = ( scaleValue - static_cast<Real>( 1.0 ) ) / scaleValue;

    Real srcValue = std::max( minValue, std::min( maxValue, static_cast<Real>( src ) ) );

    return sign_cast<Target>( round_cast<SignedTarget>( srcValue * scaleValue ) );
}

// fixed point -> floating point
// sign_cast + downscale
template <typename Target, typename Source>
typename std::enable_if<std::is_floating_point<Target>::value && std::is_integral<Source>::value, Target>::type
convert_to( Source src )
{
    using SignedSource = typename std::make_signed<Source>::type;
    using Real         = typename real_promotion<Target, sizeof( Source ) * 8>::type;

    const SignedSource srcValue   = sign_cast<SignedSource>( src );
    const Real         scaleValue = static_cast<Real>( 1.0 ) / ( 1ull << ( sizeof( Source ) * 8 - 1 ) );

    return static_cast<Target>( scaleValue * static_cast<Real>( srcValue ) );
}

// fixed point -> fixed point
// sign_cast, shift_cast
template <typename Target, typename Source>
typename std::enable_if<std::is_integral<Target>::value && std::is_integral<Source>::value, Target>::type convert_to(
    Source src )
{
    using sign_adjusted_Source = typename std::conditional<std::is_signed<Target>::value,
                                                           typename std::make_signed<Source>::type,
                                                           typename std::make_unsigned<Source>::type>::type;


    return shift_cast<Target>( sign_cast<sign_adjusted_Source>( src ) );
}


template <class T>
struct intermediate
{
};

template <endian e>
struct intermediate<::pcm::format::tag<::pcm::floating_point, ::pcm::_32bit, e>>
{
    using type = float;
};

template <endian e>
struct intermediate<::pcm::format::tag<::pcm::floating_point, ::pcm::_64bit, e>>
{
    using type = double;
};

template <endian e>
struct intermediate<::pcm::format::tag<::pcm::signed_integer, ::pcm::_8bit, e>>
{
    using type = int8_t;
};

template <endian e>
struct intermediate<::pcm::format::tag<::pcm::signed_integer, ::pcm::_16bit, e>>
{
    using type = int16_t;
};

template <endian e>
struct intermediate<::pcm::format::tag<::pcm::signed_integer, ::pcm::_24bit, e>>
{
    using type = int32_t;
};

template <endian e>
struct intermediate<::pcm::format::tag<::pcm::signed_integer, ::pcm::_32bit, e>>
{
    using type = int32_t;
};

template <endian e>
struct intermediate<::pcm::format::tag<::pcm::signed_integer, ::pcm::_64bit, e>>
{
    using type = int64_t;
};

template <endian e>
struct intermediate<::pcm::format::tag<::pcm::unsigned_integer, ::pcm::_8bit, e>>
{
    using type = uint8_t;
};

template <endian e>
struct intermediate<::pcm::format::tag<::pcm::unsigned_integer, ::pcm::_16bit, e>>
{
    using type = uint16_t;
};

template <endian e>
struct intermediate<::pcm::format::tag<::pcm::unsigned_integer, ::pcm::_24bit, e>>
{
    using type = uint32_t;
};

template <endian e>
struct intermediate<::pcm::format::tag<::pcm::unsigned_integer, ::pcm::_32bit, e>>
{
    using type = uint32_t;
};

template <endian e>
struct intermediate<::pcm::format::tag<::pcm::unsigned_integer, ::pcm::_64bit, e>>
{
    using type = uint64_t;
};


// equivalent of std::copy_n with the additional garanty that iterators will be incremented exactly n times
template <class InputIterator, class Size, class OutputIterator>
OutputIterator increment_n_copy_n( InputIterator in, Size size, OutputIterator out )
{
    for ( Size i = 0; i != size; ++i, ++in, ++out )
        *out = *in;
    return out;
}

template <class Format>
struct byte_array
{
    using intermediate_t = typename intermediate<Format>::type;

    static const auto is_native_endian = get_endian( Format{} ) == native_endian;
    static const auto num_bytes        = get_bitwidth( Format{} ) / 8;
    static const auto padding          = little_endian == native_endian ? sizeof( intermediate_t ) - num_bytes : 0;
    static const auto index            = is_native_endian ? padding : padding + num_bytes;

    template <class InputIterator>
    byte_array( InputIterator iter )
    : m_data( 0 )
    {
        increment_n_copy_n( iter, num_bytes, begin() );
    }
    byte_array( intermediate_t val )
    : m_data( val )
    {
    }

    using iterator = typename std::conditional<is_native_endian, char*, std::reverse_iterator<char*>>::type;

    using const_iterator =
        typename std::conditional<is_native_endian, const char*, std::reverse_iterator<const char*>>::type;


    auto begin() -> iterator
    {
        return iterator( &m_data.byte[index] );
    }

    auto end() -> iterator
    {
        return begin() + size();
    }

    auto begin() const -> const_iterator
    {
        return const_iterator( &m_data.byte[index] );
    }

    auto end() const -> const_iterator
    {
        return begin() + size();
    }

    auto size() const -> size_t
    {
        return num_bytes;
    }

    auto value() const -> intermediate_t
    {
        return m_data.value;
    }

private:
    union data
    {
        data( intermediate_t val )
        : value( val )
        {
        }

        intermediate_t value;
        char           byte[sizeof( intermediate_t )];
    } m_data;
};

template <class Value, class Iterator, class Format>
Value read_impl( Iterator iter )
{
    auto data = byte_array<Format>( iter );
    return convert_to<Value>( data.value() );
}


template <class Value, class Iterator, class Format>
Value read_impl( Iterator iter, const Format& )
{
    return read_impl<Value, Iterator, Format>( iter );
}


template <typename Value, typename Iterator, typename... Tags>
auto make_read_impls( const std::tuple<Tags...>& ) -> std::array<Value ( * )( Iterator ), sizeof...( Tags )>
{
    return {{&read_impl<Value, Iterator, Tags>...}};
}


template <class Value, class Iterator>
Value read_impl( Iterator iter, const format& fmt )
{
    static auto const impls = make_read_impls<Value, Iterator>( format::tags{} );
    return impls.at( get_index( fmt ) )( iter );
}


template <class Value, class Iterator, class Format>
void write_impl( Iterator iter, Value val )
{
    auto data = byte_array<Format>{convert_to<typename intermediate<Format>::type>( val )};
    std::copy_n( data.begin(), data.size(), iter );
}

template <class Value, class Iterator, class Format>
void write_impl( Iterator iter, Value val, const Format& )
{
    write_impl<Value, Iterator, Format>( iter, val );
}


template <typename Value, typename Iterator, typename... Tags>
auto make_write_impls( const std::tuple<Tags...>& ) -> std::array<void ( * )( Iterator, Value ), sizeof...( Tags )>
{
    return {{&write_impl<Value, Iterator, Tags>...}};
}

template <class Value, class Iterator>
void write_impl( Iterator iter, Value val, const format& fmt )
{
    static auto const impls = make_write_impls<Value, Iterator>( format::tags{} );
    impls.at( get_index( fmt ) )( iter, val );
}

} // namespace detail


template <class Value, class Iterator, class Format>
Value read( Iterator iter, const Format& fmt )
{
    return detail::read_impl<Value>( iter, fmt );
}

template <class Value, class Iterator, class Format>
void write( Iterator iter, Value val, const Format& fmt )
{
    detail::write_impl( iter, val, fmt );
}


} // namespace pcm
