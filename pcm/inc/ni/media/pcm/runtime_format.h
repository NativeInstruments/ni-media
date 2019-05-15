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

#include <ni/media/pcm/compiletime_format.h>

#include <ni/media/pcm/detail/tuple_find.h>
#include <ni/media/pcm/detail/tuple_to_array.h>

#include <boost/range/algorithm/find_if.hpp>

#include <array>
#include <ostream>
#include <regex>
#include <stdexcept>
#include <type_traits>

namespace pcm
{

struct runtime_format
{
    using tags = decltype( compiletime_formats() );

    template <class Format>
    using enable_if_supported =
        std::enable_if_t<detail::tuple_find<Format, tags>::value != std::tuple_size<tags>::value>;

    runtime_format( const runtime_format& ) = default;

    template <class Format = compiletime_format<>, class = enable_if_supported<Format>>
    constexpr runtime_format( const Format& fmt = {} )
    : m_number( fmt.number() )
    , m_bitwidth( fmt.bitwidth() )
    , m_endian( fmt.endian() )
    , m_index( uint8_t( detail::tuple_find<Format, tags>::value ) )
    {
    }

    runtime_format( number_type n, bitwidth_type b, endian_type e = native_endian )
    {
        static auto const formats = detail::tuple_to_array<runtime_format>( tags{} );

        auto it = boost::find_if( formats, [n, b, e]( const auto& fmt ) {
            return fmt.number() == n && fmt.bitwidth() == b && fmt.endian() == e;
        } );

        if ( it == formats.end() )
            throw std::runtime_error( "Invalid format" );

        *this /*is*/ = *it;
    }

    runtime_format( number_type n, size_t bits, endian_type e = native_endian )
    {
        auto bw = bitwidth_type{};

        switch ( bits )
        {
            case 8:
                bw = _8bit;
                break;
            case 16:
                bw = _16bit;
                break;
            case 24:
                bw = _24bit;
                break;
            case 32:
                bw = _32bit;
                break;
            case 64:
                bw = _64bit;
                break;
            default:
                throw std::runtime_error( "Invalid bitwidth: " + std::to_string( bw ) );
        };

        *this = runtime_format( n, bw, e );
    }

    // TODO c++17: replace with std::string_view
    runtime_format( const std::string& str )
    {
        std::regex  regex( "([fsu])([0-9]{1,2})(le|be|ne)" );
        std::smatch match;

        if ( std::regex_match( str, match, regex ) && ( match.size() == 4 ) )
        {
            const auto to_number = []( const auto& str ) {
                return str == "f" ? floating_point : str == "s" ? signed_integer : unsigned_integer;
            };

            const auto to_bitwidth = []( const auto& str ) { return std::stoul( str ); };

            const auto to_endian = []( const auto& str ) {
                return str == "le" ? little_endian : str == "be" ? big_endian : native_endian;
            };


            *this = runtime_format( to_number( match[1].str() ), //
                                    to_bitwidth( match[2].str() ),
                                    to_endian( match[3].str() ) );
            return;
        }

        throw std::runtime_error( "Invalid format: " + str );
    }

    auto number() const
    {
        return m_number;
    }

    auto bitwidth() const
    {
        return m_bitwidth;
    }

    auto endian() const
    {
        return m_endian;
    }

    auto index() const
    {
        return m_index;
    }

private:
    number_type   m_number;
    bitwidth_type m_bitwidth;
    endian_type   m_endian;
    uint8_t       m_index;
};

inline bool operator==( const runtime_format& lhs, const runtime_format& rhs )
{
    return lhs.index() == rhs.index();
}

inline bool operator!=( const runtime_format& lhs, const runtime_format& rhs )
{
    return lhs.index() != rhs.index();
}

inline std::ostream& operator<<( std::ostream& stream, const runtime_format& fmt )
{
    const auto number   = fmt.number() == floating_point ? "f" : fmt.number() == signed_integer ? "s" : "u";
    const auto bitwidth = fmt.bitwidth();
    const auto endian   = fmt.endian() == big_endian ? "be" : "le";

    return stream << number << bitwidth << endian;
}


inline auto runtime_formats()
{
    static auto const fs = detail::tuple_to_array<runtime_format>( compiletime_formats() );
    return fs;
}

} // namespace pcm
