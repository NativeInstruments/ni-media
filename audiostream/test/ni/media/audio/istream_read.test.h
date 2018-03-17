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

#include <ni/media/test_helper.h>

#include <ni/media/audio/ivectorstream.h>
#include <ni/media/pcm/range/converted.h>

#include <gtest/gtest.h>

#include <boost/algorithm/cxx11/iota.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/algorithm/equal.hpp>
#include <boost/range/algorithm/fill.hpp>
#include <boost/range/iterator_range.hpp>

#include <algorithm>
#include <limits>
#include <list>
#include <type_traits>
#include <vector>

namespace
{

template <class Container>
auto generate_range()
{
    auto range = std::vector<char>( 256 );
    boost::algorithm::iota( range, std::numeric_limits<char>::lowest() );

    auto format = std::is_signed<char>() ? pcm::make_format<pcm::signed_integer, pcm::_8bit>()
                                         : pcm::make_format<pcm::unsigned_integer, pcm::_8bit>();

    using Value = typename boost::range_value<Container>::type;
    return boost::copy_range<Container>( range | pcm::converted_to<Value>( format ) );
}
}


template <class Traits>
class istream_read_test : public testing::Test
{

private:
    using Value     = typename Traits::value_t;
    using Format    = typename Traits::format_t;
    using Container = std::vector<Value>;

    Container            m_source;
    audio::ivectorstream m_istream;

protected:
    istream_read_test()
    : m_source( generate_range<Container>() )
    {
        std::vector<char> buffer( m_source.size() * Format().bitwidth() / 8 );
        boost::copy( m_source, pcm::make_iterator<Value>( buffer.begin(), Format() ) );
        audio::ivectorstream::info_type info;
        info.format( Format() );
        m_istream = audio::ivectorstream( buffer, info );
    }

    bool full_unformatted_read()
    {
        std::vector<char> buffer( m_source.size() * m_istream.info().bytes_per_sample() );
        m_istream.read( buffer.data(), buffer.size() );
        auto destination = boost::copy_range<Container>( buffer | pcm::converted_to<Value>( Format() ) );
        return boost::equal( m_source, destination );
    }

    bool unformatted_read( size_t num_samples )
    {
        Container         destination;
        std::vector<char> buffer( num_samples * m_istream.info().bytes_per_sample() );

        while ( m_istream.read( buffer.data(), buffer.size() ).sample_gcount() > 0 )
        {
            auto rng = buffer | pcm::converted_to<Value>( Format() );
            destination.insert(
                destination.end(), rng.begin(), std::next( rng.begin(), (ptrdiff_t) m_istream.sample_gcount() ) );
        }

        return boost::equal( m_source, destination );
    }


    bool full_range_read()
    {
        Container destination( m_source.size() );
        m_istream >> destination;
        return boost::equal( m_source, destination );
    }

    bool range_read( size_t num_samples )
    {
        Container destination;
        Container buffer( num_samples );

        while ( m_istream >> buffer )
            destination.insert( destination.end(), buffer.begin(), buffer.end() );

        return std::equal( m_source.begin(), m_source.end(), destination.begin() );
    }

    bool full_value_read()
    {
        Container destination( m_source.size() );

        for ( auto& value : destination )
            m_istream >> value;

        return boost::equal( m_source, destination );
    }

    bool value_read()
    {
        Container destination;
        Value     value;

        while ( m_istream >> value )
            destination.push_back( value );

        return boost::equal( m_source, destination );
    }

    auto const& stream()
    {
        return m_istream;
    }

    void clear()
    {
        m_istream.clear();
        m_istream.seekg( 0 );
    }

private:
};

TYPED_TEST_CASE_P( istream_read_test );


TYPED_TEST_P( istream_read_test, value_read_test )
{
    this->clear();
    EXPECT_TRUE( this->full_value_read() );
    EXPECT_TRUE( this->stream().good() );
    EXPECT_FALSE( this->stream().eof() );
    EXPECT_FALSE( this->stream().fail() );

    this->clear();
    EXPECT_TRUE( this->value_read() );
    EXPECT_FALSE( this->stream().good() );
    EXPECT_TRUE( this->stream().eof() );
    EXPECT_TRUE( this->stream().fail() );
}


TYPED_TEST_P( istream_read_test, unformatted_read_test )
{
    // full stream
    this->clear();
    EXPECT_TRUE( this->full_unformatted_read() );
    EXPECT_TRUE( this->stream().good() );
    EXPECT_FALSE( this->stream().eof() );
    EXPECT_FALSE( this->stream().fail() );

    // aligned, blocksize = 1
    this->clear();
    EXPECT_TRUE( this->unformatted_read( 1 ) );
    EXPECT_FALSE( this->stream().good() );
    EXPECT_TRUE( this->stream().eof() );
    EXPECT_TRUE( this->stream().fail() );

    // aligned, blocksize = 8
    this->clear();
    EXPECT_TRUE( this->unformatted_read( 8 ) );
    EXPECT_FALSE( this->stream().good() );
    EXPECT_TRUE( this->stream().eof() );
    EXPECT_TRUE( this->stream().fail() );

    // unaligned, blocksize = 7
    this->clear();
    EXPECT_TRUE( this->unformatted_read( 7 ) );
    EXPECT_FALSE( this->stream().good() );
    EXPECT_TRUE( this->stream().eof() );
    EXPECT_TRUE( this->stream().fail() );

    // unaligned, blocksize = 335
    this->clear();
    EXPECT_TRUE( this->unformatted_read( 335 ) );
    EXPECT_FALSE( this->stream().good() );
    EXPECT_TRUE( this->stream().eof() );
    EXPECT_TRUE( this->stream().fail() );
}

TYPED_TEST_P( istream_read_test, range_read_test )
{
    // full stream
    this->clear();
    EXPECT_TRUE( this->full_range_read() );
    EXPECT_TRUE( this->stream().good() );
    EXPECT_FALSE( this->stream().eof() );
    EXPECT_FALSE( this->stream().fail() );

    // aligned, blocksize = 1
    this->clear();
    EXPECT_TRUE( this->range_read( 1 ) );
    EXPECT_FALSE( this->stream().good() );
    EXPECT_TRUE( this->stream().eof() );
    EXPECT_TRUE( this->stream().fail() );

    // aligned, blocksize = 8
    this->clear();
    EXPECT_TRUE( this->range_read( 8 ) );
    EXPECT_FALSE( this->stream().good() );
    EXPECT_TRUE( this->stream().eof() );
    EXPECT_TRUE( this->stream().fail() );

    // unaligned, blocksize = 7
    this->clear();
    EXPECT_TRUE( this->range_read( 7 ) );
    EXPECT_FALSE( this->stream().good() );
    EXPECT_TRUE( this->stream().eof() );
    EXPECT_TRUE( this->stream().fail() );

    // unaligned, blocksize = 335
    this->clear();
    EXPECT_TRUE( this->range_read( 335 ) );
    EXPECT_FALSE( this->stream().good() );
    EXPECT_TRUE( this->stream().eof() );
    EXPECT_TRUE( this->stream().fail() );
}


REGISTER_TYPED_TEST_CASE_P( istream_read_test, value_read_test, unformatted_read_test, range_read_test );


template <class Value, class Format>
struct istream_read_traits
{
    using value_t  = Value;
    using format_t = Format;
};

template <class Value, class Formats>
struct make_istream_read_test
{
};

template <class Value, class... Formats>
struct make_istream_read_test<Value, std::tuple<Formats...>>
{
    using type = testing::Types<istream_read_traits<Value, Formats>...>;
};

template <class Value>
using make_istream_read_test_t = typename make_istream_read_test<Value, pcm::format::tags>::type;
