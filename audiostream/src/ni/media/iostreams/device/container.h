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

#include <ni/media/iostreams/positioning.h>

#include <boost/config.hpp> // BOOST_NO_STDC_NAMESPACE.
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/detail/ios.hpp> // failure.
#include <boost/iostreams/positioning.hpp>

#include <algorithm> // copy, min.
#include <cassert>

// Must come last.
#include <boost/iostreams/detail/config/disable_warnings.hpp>

namespace boostext
{
namespace iostreams
{

template <class Container>
class container_device
{
public:
    using char_type = typename Container::value_type;
    using category  = boost::iostreams::seekable_device_tag;

    container_device( Container container )
    : m_container( std::move( container ) )
    {
    }

    std::streamsize read( char_type* s, std::streamsize n )
    {
        auto amount = std::streamsize( m_container.size() - m_pos );
        auto result = std::min( n, amount );
        if ( result != 0 )
        {
            std::copy( m_container.begin() + m_pos, m_container.begin() + m_pos + result, s );
            m_pos += result;
            return result;
        }
        else
        {
            return -1; // EOF
        }
    }

    std::streamsize write( const char_type* s, std::streamsize n )
    {
        std::streamsize result = 0;
        if ( m_pos != m_container.size() )
        {
            auto amount = std::streamsize( m_container.size() - m_pos );
            result      = std::min( n, amount );
            std::copy( s, s + result, m_container.begin() + m_pos );
            m_pos += result;
        }
        if ( result < n )
        {
            m_container.insert( m_container.end(), s, s + n );
            m_pos = m_container.size();
        }
        return n;
    }

    std::streampos seek( boost::iostreams::stream_offset off, BOOST_IOS::seekdir way )
    {
        auto beg = std::streampos( 0 );
        auto pos = std::streampos( m_pos );
        auto end = std::streampos( m_container.size() );

        m_pos = size_t( absolute_position( pos, beg, end, off, way ) );
        return m_pos;
    }

    Container& container()
    {
        return m_container;
    }

private:
    Container m_container;
    size_t    m_pos = 0;
};

template <class Container>
class container_source : private container_device<Container>
{
    using base_type = container_device<Container>;

public:
    using char_type = typename base_type::char_type;

    struct category : boost::iostreams::input_seekable, boost::iostreams::device_tag
    {
    };

    using base_type::read;
    using base_type::seek;

    using base_type::base_type;
};

template <class Container>
class container_sink : private container_device<Container>
{
    using base_type = container_device<Container>;

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
