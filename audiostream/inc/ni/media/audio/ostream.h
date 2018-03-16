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

#include <ni/media/audio/ostream_info.h>
#include <ni/media/audio/streambuf.h>

#include <ni/media/pcm/algorithm/copy.h>
#include <ni/media/pcm/iterator.h>

#include <boost/range/difference_type.hpp>
#include <boost/range/has_range_iterator.hpp>
#include <boost/range/value_type.hpp>

#include <algorithm>
#include <iterator>
#include <memory>
#include <ostream>
#include <type_traits>

namespace audio
{

class ostream : protected std::ostream
{
public:
    using info_type = ostream_info;

    // - std::ostream

    using char_type   = std::ostream::char_type;
    using traits_type = std::ostream::traits_type;
    using int_type    = std::ostream::int_type;
    using pos_type    = std::ostream::pos_type;
    using off_type    = std::ostream::off_type;

    using std::ostream::operator bool;
    using std::ostream::operator!;

    auto write( const char_type* s, std::streamsize n ) -> ostream&;

    template <class Value>
    auto operator<<( Value val ) -> std::enable_if_t<std::is_arithmetic<Value>::value, ostream&>;

    template <class Range>
    auto operator<<( const Range& rng ) -> std::enable_if_t<boost::has_range_iterator<Range>::value, ostream&>;

    using std::ostream::rdstate;
    using std::ostream::clear;
    using std::ostream::setstate;
    using std::ostream::good;
    using std::ostream::eof;
    using std::ostream::fail;
    using std::ostream::bad;


    using std::ostream::tellp;
    auto frame_tellp() -> pos_type;
    auto sample_tellp() -> pos_type;

    // - stream_info

    virtual auto info() const -> const info_type&;

protected:
    ostream( std::unique_ptr<streambuf>, std::unique_ptr<info_type> );

    ostream( const ostream& ) = delete;
    ostream& operator=( const ostream& ) = delete;

    ostream( ostream&& );
    ostream& operator=( ostream&& );

private:
    std::unique_ptr<streambuf> m_streambuf;
    std::unique_ptr<info_type> m_info;
};

//----------------------------------------------------------------------------------------------------------------------

template <class Value>
auto ostream::operator<<( Value val ) -> std::enable_if_t<std::is_arithmetic<Value>::value, ostream&>
{
    std::array<char, 8> temp;
    pcm::write( temp.data(), val, m_info->format() );
    std::ostream::write( temp.data(), m_info->bytes_per_sample() );
    return *this;
}

//----------------------------------------------------------------------------------------------------------------------

template <class Range>
auto ostream::operator<<( const Range& rng ) -> std::enable_if_t<boost::has_range_iterator<Range>::value, ostream&>
{
    // TODO fast pcm conversion
    for ( auto val : rng )
        *this << val;

    return *this;
}
}
