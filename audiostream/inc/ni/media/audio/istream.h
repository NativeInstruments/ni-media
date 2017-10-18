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

#include <ni/media/audio/istream_info.h>

#include <ni/media/pcm/algorithm/copy_n.h>
#include <ni/media/pcm/iterator.h>

#include <boost/range/difference_type.hpp>
#include <boost/range/has_range_iterator.hpp>
#include <boost/range/value_type.hpp>

#include <algorithm>
#include <istream>
#include <iterator>
#include <memory>
#include <type_traits>

namespace audio
{

class istream : protected std::istream
{
public:
    istream();

    template <class AudioSource>
    istream( AudioSource source );

    istream( const istream& ) = delete;
    istream( istream&& );

    istream& operator=( istream& ) = delete;
    istream& operator              =( istream&& );

    // - std::istream

    using char_type   = std::istream::char_type;
    using traits_type = std::istream::traits_type;
    using int_type    = std::istream::int_type;
    using pos_type    = std::istream::pos_type;
    using off_type    = std::istream::off_type;

    using std::istream::operator bool;
    using std::istream::operator!;

    template <class Value>
    auto operator>>( Value& val ) -> std::enable_if_t<std::is_arithmetic<Value>::value, istream&>;

    template <class Range>
    auto operator>>( Range&& rng ) -> std::enable_if_t<boost::has_range_iterator<Range>::value, istream&>;

    using std::istream::rdstate;
    using std::istream::clear;
    using std::istream::setstate;
    using std::istream::good;
    using std::istream::eof;
    using std::istream::fail;
    using std::istream::bad;

    auto read( char_type* s, std::streamsize n ) -> istream&;

    auto seekg( pos_type pos ) -> istream&;
    auto sample_seekg( pos_type pos ) -> istream&;
    auto frame_seekg( pos_type pos ) -> istream&;

    auto seekg( off_type off, std::ios_base::seekdir dir ) -> istream&;
    auto sample_seekg( off_type off, std::ios_base::seekdir dir ) -> istream&;
    auto frame_seekg( off_type off, std::ios_base::seekdir dir ) -> istream&;

    using std::istream::gcount;
    auto sample_gcount() const -> std::streamsize;
    auto frame_gcount() const -> std::streamsize;

    using std::istream::tellg;
    auto frame_tellg() -> pos_type;
    auto sample_tellg() -> pos_type;

    using std::istream::rdbuf;

    // - stream_info

    using info_type = istream_info;

    virtual auto info() const -> const info_type&;

private:
    std::unique_ptr<info_type>      m_info;
    std::unique_ptr<std::streambuf> m_streambuf;
};

//----------------------------------------------------------------------------------------------------------------------

template <class Value>
auto istream::operator>>( Value& val ) -> std::enable_if_t<std::is_arithmetic<Value>::value, istream&>
{
    std::array<char, 8> temp;
    if ( read( temp.data(), m_info->bytes_per_sample() ) )
        val = pcm::read<Value>( temp.data(), m_info->format() );
    return *this;
}

//----------------------------------------------------------------------------------------------------------------------

template <class Range>
auto istream::operator>>( Range&& rng ) -> std::enable_if_t<boost::has_range_iterator<Range>::value, istream&>
{
    using Value      = typename boost::range_value<Range>::type;
    using Difference = typename boost::range_difference<Range>::type;

    auto beg = std::begin( rng );
    auto end = std::end( rng );

    const auto samples_requested = std::distance( beg, end );
    if ( samples_requested == 0 || fail() )
        return *this;

    const auto samples_available = Difference( m_info->num_samples() - sample_tellg() );
    if ( samples_available == 0 || eof() )
    {
        setstate( eofbit | failbit );
        return *this;
    }

    auto iter = pcm::make_iterator<Value>( std::istreambuf_iterator<char>( rdbuf() ), m_info->format() );
    std::fill( pcm::copy_n( iter, std::min( samples_requested, samples_available ), beg ), end, Value{} );

    if ( samples_available < samples_requested )
    {
        setstate( rdstate() | eofbit );
    }

    return *this;
}

} // namespace audio
