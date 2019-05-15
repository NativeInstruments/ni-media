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

#include <gtest/gtest.h>

#include <ni/media/audio/ofstream.h>

//----------------------------------------------------------------------------------------------------------------------

TEST( ni_media_audio_ofstream, default_constructor )
{
    audio::ofstream is;

    EXPECT_TRUE( is.bad() );
}

//----------------------------------------------------------------------------------------------------------------------

TEST( ni_media_audio_ofstream, move_constructor )
{
    audio::ofstream is1;

    audio::ofstream is2( std::move( is1 ) );
    EXPECT_TRUE( is2.bad() );
}

//----------------------------------------------------------------------------------------------------------------------

TEST( ni_media_audio_ofstream, move_assignment )
{
    audio::ofstream is1, is2;
    is2 = std::move( is1 );

    EXPECT_TRUE( is2.bad() );
}

//----------------------------------------------------------------------------------------------------------------------
