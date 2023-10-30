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

#include <ni/media/iostreams/positioning.h>

#include <boost/config.hpp> // BOOST_NO_STDC_NAMESPACE.
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/detail/ios.hpp> // failure.
#include <boost/iostreams/positioning.hpp>

#include <cassert>

// Must come last.
#include <boost/iostreams/detail/config/disable_warnings.hpp>

namespace boostext
{
namespace iostreams
{

template <class Impl>
class custom_device
{
public:
    using char_type = typename Impl::char_type;
    using category  = boost::iostreams::seekable_device_tag;

    custom_device( custom_device&& ) = default;
    
    custom_device( std::unique_ptr<Impl> impl )
    : m_impl( std::move(impl) )
    {
    }

    std::streamsize read( char_type* s, std::streamsize n )
    {
        return m_impl->read( s, n );
    }

    std::streamsize write( const char_type* s, std::streamsize n )
    {
        return m_impl->write( s, n );
    }

    std::streampos seek( boost::iostreams::stream_offset off, BOOST_IOS::seekdir way )
    {
        return m_impl->seek( off, way );
    }

private:
    std::unique_ptr<Impl> m_impl;
};

template <class Impl>
class custom_source : private custom_device<Impl>
{
    using base_type = custom_device<Impl>;

public:
    using char_type = typename base_type::char_type;

    struct category : boost::iostreams::input_seekable, boost::iostreams::device_tag
    {
    };

    using base_type::read;
    using base_type::seek;

    using base_type::base_type;
};

template <class Impl>
class custom_sink : private custom_device<Impl>
{
    using base_type = custom_device<Impl>;

public:
    using char_type = typename base_type::char_type;

    struct category : boost::iostreams::output_seekable, boost::iostreams::device_tag
    {
    };

    using base_type::seek;
    using base_type::write;

    using base_type::base_type;
};
}
}


#include <boost/iostreams/detail/config/enable_warnings.hpp>
