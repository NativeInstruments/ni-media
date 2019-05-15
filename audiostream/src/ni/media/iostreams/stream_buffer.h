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

#include <ni/media/audio/streambuf.h>

#include <boost/iostreams/detail/streambuf.hpp>

#undef BOOST_IOSTREAMS_BASIC_STREAMBUF
#define BOOST_IOSTREAMS_BASIC_STREAMBUF( ch, tr ) audio::basic_streambuf<ch, tr>

#include <boost/iostreams/stream_buffer.hpp>

#include <type_traits>

// this is a workaround for boost::iostreams::stream_buffer lack of support for move semantics
// can be removed once boost::iostreams support move semantics
#if 1

namespace detail
{
template <class T>
class device_holder
{
public:
    device_holder( T device )
    : m_device( std::move( device ) )
    {
    }

    auto ref()
    {
        return boost::ref( m_device );
    }

private:
    T m_device;
};

template <class T>
class stream_buffer_owner : private device_holder<T>,
                            public boost::iostreams::stream_buffer<boost::reference_wrapper<T>>
{
    using holder_t = device_holder<T>;
    using base_t   = typename boost::iostreams::stream_buffer<boost::reference_wrapper<T>>;

public:
    template <class... Args>
    stream_buffer_owner( T t, Args&&... args )
    : holder_t( std::move( t ) )
    , base_t( this->ref(), std::forward<Args>( args )... )
    {
    }

    stream_buffer_owner( const stream_buffer_owner& ) = delete;
    stream_buffer_owner( stream_buffer_owner&& )      = delete;
    stream_buffer_owner& operator=( const stream_buffer_owner& ) = delete;
    stream_buffer_owner& operator=( stream_buffer_owner&& ) = delete;
};
} // namespace detail

//----------------------------------------------------------------------------------------------------------------------

template <class T>
using stream_buffer = detail::stream_buffer_owner<T>;

#else

template <class T>
using stream_buffer = boost::iostreams::stream_buffer<T>;

#endif

template <class Device>
auto make_stream_buffer( Device&& device, size_t frames_per_buffer = 4096 )
{
    auto bytes_per_buffer = frames_per_buffer * device.info().bytes_per_frame();
    return std::make_unique<stream_buffer<std::decay_t<Device>>>( std::forward<Device>( device ), bytes_per_buffer, 4 );
}
