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

#include <gtest/gtest.h>

#include <ni/media/audio/ivectorstream.h>

#include <boost/range/algorithm/equal.hpp>

//----------------------------------------------------------------------------------------------------------------------

TEST( ni_media_audio_ivectorstream, default_constructor )
{
    audio::ivectorstream is;

    EXPECT_EQ( -1, is.tellg() );
    EXPECT_TRUE( is.bad() );
}

//----------------------------------------------------------------------------------------------------------------------


TEST( ni_media_audio_ivectorstream, vector_constructor )
{
    std::vector<char> in{1, 2, 3, 4};
    std::vector<char> out( in.size() );

    audio::ivectorstream is( in, audio::istream_info() );

    EXPECT_EQ( 0, is.tellg() );
    EXPECT_FALSE( is.eof() );

    is.read( out.data(), in.size() );

    EXPECT_TRUE( boost::equal( in, out ) );
    EXPECT_EQ( audio::ivectorstream::pos_type(in.size()), is.tellg() );
    EXPECT_FALSE( is.eof() );

    is.read( out.data(), 1 );

    EXPECT_EQ( -1, is.tellg() );
    EXPECT_TRUE( is.eof() );
}

//----------------------------------------------------------------------------------------------------------------------

TEST( ni_media_audio_ivectorstream, move_constructor )
{
    std::vector<char> in{1, 2, 3, 4};
    std::vector<char> out( in.size() );

    audio::ivectorstream is1( in, audio::istream_info() );

    audio::ivectorstream is2( std::move( is1 ) );

    EXPECT_EQ( 0, is2.tellg() );
    EXPECT_FALSE( is2.eof() );

    is2.read( out.data(), in.size() );

    EXPECT_TRUE( boost::equal( in, out ) );
    EXPECT_EQ( audio::ivectorstream::pos_type(in.size()), is2.tellg() );
    EXPECT_FALSE( is2.eof() );

    is2.read( out.data(), 1 );

    EXPECT_EQ( -1, is2.tellg() );
    EXPECT_TRUE( is2.eof() );
}

//----------------------------------------------------------------------------------------------------------------------

TEST( ni_media_audio_ivectorstream, move_assignment )
{
    std::vector<char> in{1, 2, 3, 4};
    std::vector<char> out( in.size() );

    audio::ivectorstream is1( in, audio::istream_info() );
    audio::ivectorstream is2;

    is2 = std::move( is1 );

    EXPECT_EQ( 0, is2.tellg() );
    EXPECT_FALSE( is2.eof() );

    is2.read( out.data(), in.size() );

    EXPECT_TRUE( boost::equal( in, out ) );
    EXPECT_EQ( audio::ivectorstream::pos_type(in.size()), is2.tellg() );
    EXPECT_FALSE( is2.eof() );

    is2.read( out.data(), 1 );

    EXPECT_EQ( -1, is2.tellg() );
    EXPECT_TRUE( is2.eof() );
}
