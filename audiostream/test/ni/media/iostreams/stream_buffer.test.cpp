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

#include <ni/media/iostreams/stream_buffer.h>

#include <type_traits>

class device
{
public:
    using char_type = char;
    struct category : boost::iostreams::input, boost::iostreams::device_tag
    {
    };

    auto read( char_type*, std::streamsize ) -> std::streamsize
    {
        return {};
    }
};

class move_only_device : public device
{
public:
    move_only_device( const move_only_device& ) = delete;
    move_only_device( move_only_device&& )      = default;
};

template <class T>
using is_stream_buffer_constructible_from = std::is_constructible<stream_buffer<std::decay_t<T>>, T>;

TEST( ni_media_stream_buffer, prerequisites )
{
    ASSERT_TRUE( std::is_copy_constructible<device>() );
    ASSERT_TRUE( std::is_move_constructible<device>() );

    ASSERT_FALSE( std::is_copy_constructible<move_only_device>() );
    ASSERT_TRUE( std::is_move_constructible<move_only_device>() );
}

TEST( ni_media_stream_buffer, constructible_from_device )
{
    EXPECT_TRUE( is_stream_buffer_constructible_from<device>() );
}

TEST( ni_media_stream_buffer, constructible_from_move_only_device )
{
    EXPECT_TRUE( is_stream_buffer_constructible_from<move_only_device>() );
}
