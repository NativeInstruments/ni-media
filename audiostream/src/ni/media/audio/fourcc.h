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

#pragma once

#include <cstdint>
#include <string>

constexpr inline uint32_t big_endian_fourcc( char const p[5] )
{
    return ( p[0] << 24 ) | ( p[1] << 16 ) | ( p[2] << 8 ) | p[3];
}

constexpr inline uint32_t little_endian_fourcc( char const p[5] )
{
    return ( p[3] << 24 ) | ( p[2] << 16 ) | ( p[1] << 8 ) | p[0];
}

inline std::string fourcc_to_string( uint32_t x )
{
    return std::string( reinterpret_cast<const char*>( &x ), sizeof( x ) );
}
