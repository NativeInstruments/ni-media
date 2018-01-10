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

#include <ni/media/pcm/description.h>

#include <tuple>
#include <type_traits>

namespace pcm
{

template <number_type n = signed_integer, bitwidth_type b = _8bit, endian_type e = native_endian>
struct compiletime_format
{
    constexpr compiletime_format()                            = default;
    constexpr compiletime_format( const compiletime_format& ) = default;

    constexpr auto number() const
    {
        return n;
    }

    constexpr auto bitwidth() const
    {
        return b;
    }

    constexpr auto endian() const
    {
        return e;
    }
};

template <number_type ln, bitwidth_type lb, endian_type le, number_type rn, bitwidth_type rb, endian_type re>
constexpr auto operator==( compiletime_format<ln, lb, le>, compiletime_format<rn, rb, re> )
{
    return std::is_same<compiletime_format<ln, lb, le>, compiletime_format<rn, rb, re>>::value;
}

template <number_type ln, bitwidth_type lb, endian_type le, number_type rn, bitwidth_type rb, endian_type re>
constexpr auto operator!=( compiletime_format<ln, lb, le>, compiletime_format<rn, rb, re> )
{
    return !std::is_same<compiletime_format<ln, lb, le>, compiletime_format<rn, rb, re>>::value;
}

inline constexpr auto compiletime_formats()
{
    return std::tuple<compiletime_format<signed_integer, _8bit, big_endian>,
                      compiletime_format<signed_integer, _8bit, little_endian>,
                      compiletime_format<signed_integer, _16bit, big_endian>,
                      compiletime_format<signed_integer, _16bit, little_endian>,
                      compiletime_format<signed_integer, _24bit, big_endian>,
                      compiletime_format<signed_integer, _24bit, little_endian>,
                      compiletime_format<signed_integer, _32bit, big_endian>,
                      compiletime_format<signed_integer, _32bit, little_endian>,
                      compiletime_format<signed_integer, _64bit, big_endian>,
                      compiletime_format<signed_integer, _64bit, little_endian>,
                      compiletime_format<unsigned_integer, _8bit, big_endian>,
                      compiletime_format<unsigned_integer, _8bit, little_endian>,
                      compiletime_format<unsigned_integer, _16bit, big_endian>,
                      compiletime_format<unsigned_integer, _16bit, little_endian>,
                      compiletime_format<unsigned_integer, _24bit, big_endian>,
                      compiletime_format<unsigned_integer, _24bit, little_endian>,
                      compiletime_format<unsigned_integer, _32bit, big_endian>,
                      compiletime_format<unsigned_integer, _32bit, little_endian>,
                      compiletime_format<unsigned_integer, _64bit, big_endian>,
                      compiletime_format<unsigned_integer, _64bit, little_endian>,
                      compiletime_format<floating_point, _32bit, big_endian>,
                      compiletime_format<floating_point, _32bit, little_endian>,
                      compiletime_format<floating_point, _64bit, big_endian>,
                      compiletime_format<floating_point, _64bit, little_endian>>();
}


} // namespace pcm
