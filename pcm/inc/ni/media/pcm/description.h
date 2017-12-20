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

#include <boost/detail/endian.hpp> // BOOST_LITTLE_ENDIAN

#include <cstdint>

namespace pcm
{

enum number_type : uint8_t
{
    signed_integer = 0,
    unsigned_integer,
    floating_point
};

enum bitwidth_type : uint8_t
{
    _8bit  = 8,
    _16bit = 16,
    _24bit = 24,
    _32bit = 32,
    _64bit = 64
};

enum endian_type : uint8_t
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

} // namespace pcm
