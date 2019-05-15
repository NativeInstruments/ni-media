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
#include <ni/media/pcm/runtime_format.h>

namespace pcm
{

using format = runtime_format;


template <number_type n, bitwidth_type b, endian_type e = native_endian>
constexpr auto make_format()
{
    return compiletime_format<n, b, e>{};
}

inline auto make_format( number_type n, bitwidth_type b, endian_type e )
{
    return runtime_format{n, b, e};
}


// the following functions are for backwards compatibility and are not needed anymore
template <class Format>
constexpr auto get_bitwidth( const Format& fmt )
{
    return fmt.bitwidth();
}

template <class Format>
constexpr auto get_number( const Format& fmt )
{
    return fmt.number();
}

template <class Format>
constexpr auto get_endian( const Format& fmt )
{
    return fmt.endian();
}

} // namespace pcm
