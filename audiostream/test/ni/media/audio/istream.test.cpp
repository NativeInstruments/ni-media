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

#include <ni/media/audio/istream.h>
#include <ni/media/audio/ivectorstream.h>
#include <ni/media/audio/source.h>

#include <ni/media/test_helper.h>

#include <type_traits>

//----------------------------------------------------------------------------------------------------------------------

TEST( ni_media_audio_istream, default_constructor )
{
    audio::istream is;

    EXPECT_EQ( -1, is.tellg() );
    EXPECT_TRUE( is.bad() );
}

//----------------------------------------------------------------------------------------------------------------------

TEST( ni_media_audio_istream, move_constructor )
{
    audio::istream is1;

    audio::istream is2( std::move( is1 ) );

    EXPECT_EQ( -1, is2.tellg() );
    EXPECT_TRUE( is2.bad() );
}

//----------------------------------------------------------------------------------------------------------------------

TEST( ni_media_audio_istream, move_assignment )
{
    audio::istream is1, is2;
    is2 = std::move( is1 );

    EXPECT_EQ( -1, is2.tellg() );
    EXPECT_TRUE( is2.bad() );
}

//----------------------------------------------------------------------------------------------------------------------

TEST( ni_media_audio_istream, construct_from_source )
{
    using Source = container_source<std::vector<char>, audio::ivectorstream::info_type>;

    auto           source = Source( {0, 1, 2, 3}, {} );
    auto           sink   = std::vector<char>{0, 0, 0, 0};
    audio::istream is( source );
    is.read( sink.data(), sink.size() );
    EXPECT_EQ( sink[0], 0 );
    EXPECT_EQ( sink[1], 1 );
    EXPECT_EQ( sink[2], 2 );
    EXPECT_EQ( sink[3], 3 );
}

//----------------------------------------------------------------------------------------------------------------------

TEST( ni_media_audio_istream, construct_from_const_source )
{
    using Source = container_source<std::vector<char>, audio::ivectorstream::info_type>;

    const auto     source = Source( {0, 1, 2, 3}, {} );
    auto           sink   = std::vector<char>{0, 0, 0, 0};
    audio::istream is( source );
    is.read( sink.data(), sink.size() );
    EXPECT_EQ( sink[0], 0 );
    EXPECT_EQ( sink[1], 1 );
    EXPECT_EQ( sink[2], 2 );
    EXPECT_EQ( sink[3], 3 );
}
//----------------------------------------------------------------------------------------------------------------------

TEST( ni_media_audio_istream, construct_from_rvalue_source )
{
    using Source = container_source<std::vector<char>, audio::ivectorstream::info_type>;

    // rvalue ref
    auto           source = Source( {0, 1, 2, 3}, {} );
    auto           sink   = std::vector<char>{0, 0, 0, 0};
    audio::istream is( std::move( source ) );
    is.read( sink.data(), sink.size() );
    EXPECT_EQ( sink[0], 0 );
    EXPECT_EQ( sink[1], 1 );
    EXPECT_EQ( sink[2], 2 );
    EXPECT_EQ( sink[3], 3 );
}
