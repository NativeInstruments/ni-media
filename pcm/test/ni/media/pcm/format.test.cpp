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

#include <gtest/gtest.h>

#include <ni/media/pcm/format.h>


TEST( pcm_format_test, constructor_default )
{
    EXPECT_NO_THROW( pcm::format{} );
}

TEST( pcm_format_test, constructor_throws_invalid_format )
{
    EXPECT_THROW( pcm::format f( pcm::floating_point, 16, pcm::little_endian ), std::runtime_error );
}

TEST( pcm_format_test, constructor_throws_invalid_bitwidth )
{
    EXPECT_THROW( pcm::format f( pcm::signed_integer, 23, pcm::little_endian ), std::runtime_error );
}


TEST( pcm_format_test, constructor_s8be )
{
    auto fc = pcm::make_format<pcm::signed_integer, pcm::_8bit, pcm::big_endian>();
    auto fr = pcm::make_format( pcm::signed_integer, pcm::_8bit, pcm::big_endian );

    EXPECT_EQ( fc, fr );
}

TEST( pcm_format_test, constructor_s8le )
{
    auto fc = pcm::make_format<pcm::signed_integer, pcm::_8bit, pcm::little_endian>();
    auto fr = pcm::make_format( pcm::signed_integer, pcm::_8bit, pcm::little_endian );

    EXPECT_EQ( fc, fr );
}


TEST( pcm_format_test, constructor_s16be )
{
    auto fc = pcm::make_format<pcm::signed_integer, pcm::_16bit, pcm::big_endian>();
    auto fr = pcm::make_format( pcm::signed_integer, pcm::_16bit, pcm::big_endian );

    EXPECT_EQ( fc, fr );
}

TEST( pcm_format_test, constructor_s16le )
{
    auto fc = pcm::make_format<pcm::signed_integer, pcm::_16bit, pcm::little_endian>();
    auto fr = pcm::make_format( pcm::signed_integer, pcm::_16bit, pcm::little_endian );

    EXPECT_EQ( fc, fr );
}

TEST( pcm_format_test, constructor_s24be )
{
    auto fc = pcm::make_format<pcm::signed_integer, pcm::_24bit, pcm::big_endian>();
    auto fr = pcm::make_format( pcm::signed_integer, pcm::_24bit, pcm::big_endian );

    EXPECT_EQ( fc, fr );
}

TEST( pcm_format_test, constructor_s24le )
{
    auto fc = pcm::make_format<pcm::signed_integer, pcm::_24bit, pcm::little_endian>();
    auto fr = pcm::make_format( pcm::signed_integer, pcm::_24bit, pcm::little_endian );

    EXPECT_EQ( fc, fr );
}

TEST( pcm_format_test, constructor_s32be )
{
    auto fc = pcm::make_format<pcm::signed_integer, pcm::_32bit, pcm::big_endian>();
    auto fr = pcm::make_format( pcm::signed_integer, pcm::_32bit, pcm::big_endian );

    EXPECT_EQ( fc, fr );
}

TEST( pcm_format_test, constructor_s32le )
{
    auto fc = pcm::make_format<pcm::signed_integer, pcm::_32bit, pcm::little_endian>();
    auto fr = pcm::make_format( pcm::signed_integer, pcm::_32bit, pcm::little_endian );

    EXPECT_EQ( fc, fr );
}

TEST( pcm_format_test, constructor_s64be )
{
    auto fc = pcm::make_format<pcm::signed_integer, pcm::_64bit, pcm::big_endian>();
    auto fr = pcm::make_format( pcm::signed_integer, pcm::_64bit, pcm::big_endian );

    EXPECT_EQ( fc, fr );
}

TEST( pcm_format_test, constructor_s64le )
{
    auto fc = pcm::make_format<pcm::signed_integer, pcm::_64bit, pcm::little_endian>();
    auto fr = pcm::make_format( pcm::signed_integer, pcm::_64bit, pcm::little_endian );

    EXPECT_EQ( fc, fr );
}

TEST( pcm_format_test, constructor_u8be )
{
    auto fc = pcm::make_format<pcm::unsigned_integer, pcm::_8bit, pcm::big_endian>();
    auto fr = pcm::make_format( pcm::unsigned_integer, pcm::_8bit, pcm::big_endian );

    EXPECT_EQ( fc, fr );
}

TEST( pcm_format_test, constructor_u8le )
{
    auto fc = pcm::make_format<pcm::unsigned_integer, pcm::_8bit, pcm::little_endian>();
    auto fr = pcm::make_format( pcm::unsigned_integer, pcm::_8bit, pcm::little_endian );

    EXPECT_EQ( fc, fr );
}

TEST( pcm_format_test, constructor_u16be )
{
    auto fc = pcm::make_format<pcm::unsigned_integer, pcm::_16bit, pcm::big_endian>();
    auto fr = pcm::make_format( pcm::unsigned_integer, pcm::_16bit, pcm::big_endian );

    EXPECT_EQ( fc, fr );
}

TEST( pcm_format_test, constructor_u16le )
{
    auto fc = pcm::make_format<pcm::unsigned_integer, pcm::_16bit, pcm::little_endian>();
    auto fr = pcm::make_format( pcm::unsigned_integer, pcm::_16bit, pcm::little_endian );

    EXPECT_EQ( fc, fr );
}

TEST( pcm_format_test, constructor_u24be )
{
    auto fc = pcm::make_format<pcm::unsigned_integer, pcm::_24bit, pcm::big_endian>();
    auto fr = pcm::make_format( pcm::unsigned_integer, pcm::_24bit, pcm::big_endian );

    EXPECT_EQ( fc, fr );
}

TEST( pcm_format_test, constructor_u24le )
{
    auto fc = pcm::make_format<pcm::unsigned_integer, pcm::_24bit, pcm::little_endian>();
    auto fr = pcm::make_format( pcm::unsigned_integer, pcm::_24bit, pcm::little_endian );

    EXPECT_EQ( fc, fr );
}

TEST( pcm_format_test, constructor_u32be )
{
    auto fc = pcm::make_format<pcm::unsigned_integer, pcm::_32bit, pcm::big_endian>();
    auto fr = pcm::make_format( pcm::unsigned_integer, pcm::_32bit, pcm::big_endian );

    EXPECT_EQ( fc, fr );
}

TEST( pcm_format_test, constructor_u32le )
{
    auto fc = pcm::make_format<pcm::unsigned_integer, pcm::_32bit, pcm::little_endian>();
    auto fr = pcm::make_format( pcm::unsigned_integer, pcm::_32bit, pcm::little_endian );

    EXPECT_EQ( fc, fr );
}

TEST( pcm_format_test, constructor_u64be )
{
    auto fc = pcm::make_format<pcm::unsigned_integer, pcm::_64bit, pcm::big_endian>();
    auto fr = pcm::make_format( pcm::unsigned_integer, pcm::_64bit, pcm::big_endian );

    EXPECT_EQ( fc, fr );
}

TEST( pcm_format_test, constructor_u64le )
{
    auto fc = pcm::make_format<pcm::unsigned_integer, pcm::_64bit, pcm::little_endian>();
    auto fr = pcm::make_format( pcm::unsigned_integer, pcm::_64bit, pcm::little_endian );

    EXPECT_EQ( fc, fr );
}

TEST( pcm_format_test, constructor_f32be )
{
    auto fc = pcm::make_format<pcm::floating_point, pcm::_32bit, pcm::big_endian>();
    auto fr = pcm::make_format( pcm::floating_point, pcm::_32bit, pcm::big_endian );

    EXPECT_EQ( fc, fr );
}

TEST( pcm_format_test, constructor_f32le )
{
    auto fc = pcm::make_format<pcm::floating_point, pcm::_32bit, pcm::little_endian>();
    auto fr = pcm::make_format( pcm::floating_point, pcm::_32bit, pcm::little_endian );

    EXPECT_EQ( fc, fr );
}

TEST( pcm_format_test, constructor_f64be )
{
    auto fc = pcm::make_format<pcm::floating_point, pcm::_64bit, pcm::big_endian>();
    auto fr = pcm::make_format( pcm::floating_point, pcm::_64bit, pcm::big_endian );

    EXPECT_EQ( fc, fr );
}

TEST( pcm_format_test, constructor_f64le )
{
    auto fc = pcm::make_format<pcm::floating_point, pcm::_64bit, pcm::little_endian>();
    auto fr = pcm::make_format( pcm::floating_point, pcm::_64bit, pcm::little_endian );

    EXPECT_EQ( fc, fr );
}
