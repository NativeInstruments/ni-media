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


namespace boostext
{
namespace iostreams
{

namespace
{

const std::streampos max_pos = std::streamoff( std::numeric_limits<std::streamsize>::max() );
}

template <class Device>
class subview_device : public Device
{
    using base_type = Device;

public:
    using char_type   = typename base_type::char_type;
    using offset_type = boost::iostreams::stream_offset;
    using base_type::base_type;


    //----------------------------------------------------------------------------------------------------------------------

    std::streampos seek( boost::iostreams::stream_offset off, BOOST_IOS::seekdir way )
    {
        const auto old_pos = base_type::seek( 0, BOOST_IOS::cur );
        const auto new_pos = absolute_position( old_pos, m_beg, m_end, off, way );

        return base_type::seek( new_pos, BOOST_IOS::beg ) - m_beg;
    }

    //----------------------------------------------------------------------------------------------------------------------

    void set_view( std::streampos beg = 0, std::streampos end = max_pos )
    {
        m_beg = beg;
        m_end = end;
    }

    //----------------------------------------------------------------------------------------------------------------------

    std::streampos tell()
    {
        return seek( 0, BOOST_IOS::cur );
    }

    //----------------------------------------------------------------------------------------------------------------------

    std::streampos beg() const
    {
        return m_beg;
    }

    //----------------------------------------------------------------------------------------------------------------------

    std::streampos end() const
    {
        return m_end;
    }

    //----------------------------------------------------------------------------------------------------------------------

    std::streampos size() const
    {
        return end() - beg();
    }

    //----------------------------------------------------------------------------------------------------------------------

private:
    std::streampos m_beg = 0;
    std::streampos m_end = max_pos;
};

//----------------------------------------------------------------------------------------------------------------------

template <class Sink>
class subview_sink : public subview_device<Sink>
{
    using base_type = subview_device<Sink>;

public:
    using base_type::base_type;
    using char_type = typename base_type::char_type;

    std::streamsize write( const char_type* s, std::streamsize n )
    {
        std::streampos pos = this->tell();

        if ( pos == std::streampos( -1 ) || pos > this->size() )
            return -1;

        return base_type::write( s, std::min( n, std::streamsize( this->size() - pos ) ) );
    }
};

//----------------------------------------------------------------------------------------------------------------------

template <class Source>
class subview_source : public subview_device<Source>
{
    using base_type = subview_device<Source>;

public:
    using base_type::base_type;
    using char_type = typename base_type::char_type;

    std::streamsize read( char_type* s, std::streamsize n )
    {
        std::streampos pos = this->tell();

        if ( pos == std::streampos( -1 ) || pos > this->size() )
            return -1;

        return base_type::read( s, std::min( n, std::streamsize( this->size() - pos ) ) );
    }
};
}
}
