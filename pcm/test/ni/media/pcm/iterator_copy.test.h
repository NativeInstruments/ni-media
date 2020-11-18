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

#include <ni/media/pcm/algorithm/copy.h>
#include <ni/media/pcm/iterator.h>
#include <ni/media/pcm/numspace.h>
#include <ni/media/pcm/range/converted.h>

#include <gtest/gtest.h>

#include <boost/range/algorithm/copy.hpp>
#include <boost/range/algorithm/equal.hpp>
#include <boost/range/algorithm/fill.hpp>
#include <boost/range/iterator_range.hpp>

#include <algorithm>
#include <list>
#include <type_traits>
#include <vector>


template <class Traits>
class PcmIteratorTest : public testing::Test
{

private:
    using Value        = typename Traits::value_t;
    using Format       = typename Traits::format_t;
    using SrcContainer = typename Traits::src_container_t;
    using PcmContainer = typename Traits::pcm_container_t;
    using DstContainer = typename Traits::dst_container_t;

    SrcContainer m_source;
    PcmContainer m_compiletime_buffer, m_runtime_buffer, m_compiletime_dispatch_buffer, m_runtime_dispatch_buffer;
    DstContainer m_compiletime_destination, m_runtime_destination, m_compiletime_dispatch_destination,
        m_runtime_dispatch_destination;

    static auto const bit_precision = 8;

    bool m_use_range = false;

protected:
    PcmIteratorTest()
    : m_source( boost::copy_range<SrcContainer>( pcm::numspace<Value>( bit_precision ) ) )
    , m_compiletime_destination( m_source.size() )
    , m_runtime_destination( m_source.size() )
    , m_compiletime_dispatch_destination( m_source.size() )
    , m_runtime_dispatch_destination( m_source.size() )
    , m_compiletime_buffer( m_source.size() * Format().bitwidth() / 8 )
    , m_runtime_buffer( m_source.size() * Format().bitwidth() / 8 )
    , m_compiletime_dispatch_buffer( m_source.size() * Format().bitwidth() / 8 )
    , m_runtime_dispatch_buffer( m_source.size() * Format().bitwidth() / 8 )
    {
    }

    void use_range( bool value )
    {
        m_use_range = value;
    }

    //---------------------------------------------------------------------------

    bool equal() const
    {
        return boost::equal( m_runtime_buffer, m_compiletime_buffer )
               && boost::equal( m_runtime_dispatch_buffer, m_compiletime_buffer );
    }


    //---------------------------------------------------------------------------

    void clear()
    {
        boost::fill( m_compiletime_buffer, uint8_t{} );
        boost::fill( m_runtime_buffer, uint8_t{} );
        boost::fill( m_compiletime_dispatch_buffer, uint8_t{} );
        boost::fill( m_runtime_dispatch_buffer, uint8_t{} );
        boost::fill( m_compiletime_destination, Value{} );
        boost::fill( m_runtime_destination, Value{} );
        boost::fill( m_compiletime_dispatch_destination, Value{} );
        boost::fill( m_runtime_dispatch_destination, Value{} );
    }


    //---------------------------------------------------------------------------

    bool compiletime_copy_to()
    {
        if ( m_use_range )
        {
            auto dest = m_compiletime_buffer | pcm::converted_to<Value>( compiletime_format() );

            return ( std::end( dest ) == boost::copy( m_source, std::begin( dest ) ) );
        }
        else
        {
            auto beg = pcm::make_iterator<Value>( m_compiletime_buffer.begin(), compiletime_format() );
            auto end = pcm::make_iterator<Value>( m_compiletime_buffer.end(), compiletime_format() );

            return ( end == std::copy( m_source.begin(), m_source.end(), beg ) );
        }
    }

    bool compiletime_copy_from()
    {
        if ( m_use_range )
        {
            auto src = m_compiletime_buffer | pcm::converted_to<Value>( compiletime_format() );

            return ( m_compiletime_destination.end() == boost::copy( src, m_compiletime_destination.begin() ) );
        }
        else
        {
            auto beg = pcm::make_iterator<Value>( m_compiletime_buffer.cbegin(), compiletime_format() );
            auto end = pcm::make_iterator<Value>( m_compiletime_buffer.cend(), compiletime_format() );

            return ( m_compiletime_destination.end() == std::copy( beg, end, m_compiletime_destination.begin() ) );
        }
    }

    bool compiletime_equal() const
    {
        return boost::equal( m_source, m_compiletime_destination );
    }

    bool runtime_copy_to()
    {
        if ( m_use_range )
        {
            auto dest = m_runtime_buffer | pcm::converted_to<Value>( runtime_format() );

            return ( std::end( dest ) == boost::copy( m_source, std::begin( dest ) ) );
        }
        else
        {
            auto beg = pcm::make_iterator<Value>( m_runtime_buffer.begin(), runtime_format() );
            auto end = pcm::make_iterator<Value>( m_runtime_buffer.end(), runtime_format() );

            return ( end == std::copy( m_source.begin(), m_source.end(), beg ) );
        }
    }

    bool runtime_copy_from()
    {
        if ( m_use_range )
        {
            auto src = m_runtime_buffer | pcm::converted_to<Value>( runtime_format() );

            return ( m_runtime_destination.end() == boost::copy( src, m_runtime_destination.begin() ) );
        }
        else
        {
            auto beg = pcm::make_iterator<Value>( m_runtime_buffer.cbegin(), runtime_format() );
            auto end = pcm::make_iterator<Value>( m_runtime_buffer.cend(), runtime_format() );

            return ( m_runtime_destination.end() == std::copy( beg, end, m_runtime_destination.begin() ) );
        }
    }

    bool runtime_equal() const
    {
        return boost::equal( m_source, m_runtime_destination );
    }

    //---------------------------------------------------------------------------

    bool compiletime_dispatch_copy_to()
    {
        if ( m_use_range )
        {
            auto dest = m_compiletime_dispatch_buffer | pcm::converted_to<Value>( compiletime_format() );

            return ( std::end( dest ) == boost::copy( m_source, std::begin( dest ) ) );
        }
        else
        {
            auto beg = pcm::make_iterator<Value>( m_compiletime_dispatch_buffer.begin(), compiletime_format() );
            auto end = pcm::make_iterator<Value>( m_compiletime_dispatch_buffer.end(), compiletime_format() );

            return ( end == std::copy( m_source.begin(), m_source.end(), beg ) );
        }
    }

    bool compiletime_dispatch_copy_from()
    {
        if ( m_use_range )
        {
            auto src = m_compiletime_dispatch_buffer | pcm::converted_to<Value>( compiletime_format() );

            return ( m_compiletime_dispatch_destination.end()
                     == boost::copy( src, m_compiletime_dispatch_destination.begin() ) );
        }
        else
        {
            auto beg = pcm::make_iterator<Value>( m_compiletime_dispatch_buffer.cbegin(), compiletime_format() );
            auto end = pcm::make_iterator<Value>( m_compiletime_dispatch_buffer.cend(), compiletime_format() );

            return ( m_compiletime_dispatch_destination.end()
                     == std::copy( beg, end, m_compiletime_dispatch_destination.begin() ) );
        }
    }

    bool compiletime_dispatch_equal() const
    {
        return boost::equal( m_source, m_compiletime_dispatch_destination );
    }

    bool runtime_dispatch_copy_to()
    {
        if ( m_use_range )
        {
            auto dest = m_runtime_dispatch_buffer | pcm::converted_to<Value>( runtime_format() );

            return ( std::end( dest ) == pcm::copy( m_source, std::begin( dest ) ) );
        }
        else
        {
            auto beg = pcm::make_iterator<Value>( m_runtime_dispatch_buffer.begin(), runtime_format() );
            auto end = pcm::make_iterator<Value>( m_runtime_dispatch_buffer.end(), runtime_format() );

            return ( end == pcm::copy( m_source.begin(), m_source.end(), beg ) );
        }
    }

    bool runtime_dispatch_copy_from()
    {
        if ( m_use_range )
        {
            auto src = m_runtime_dispatch_buffer | pcm::converted_to<Value>( runtime_format() );

            return ( m_runtime_dispatch_destination.end() == pcm::copy( src, m_runtime_dispatch_destination.begin() ) );
        }
        else
        {
            auto beg = pcm::make_iterator<Value>( m_runtime_dispatch_buffer.cbegin(), runtime_format() );
            auto end = pcm::make_iterator<Value>( m_runtime_dispatch_buffer.cend(), runtime_format() );

            return ( m_runtime_dispatch_destination.end()
                     == pcm::copy( beg, end, m_runtime_dispatch_destination.begin() ) );
        }
    }

    bool runtime_dispatch_equal() const
    {
        return boost::equal( m_source, m_runtime_dispatch_destination );
    }

private:
    auto compiletime_format() const
    {
        return Format{};
    }

    auto runtime_format() const
    {
        return pcm::runtime_format( Format{} );
    }
};

TYPED_TEST_SUITE_P( PcmIteratorTest );

TYPED_TEST_P( PcmIteratorTest, iterator_based_copy_test )
{
    this->use_range( false );
    this->clear();
    EXPECT_TRUE( this->compiletime_copy_to() );
    EXPECT_TRUE( this->compiletime_copy_from() );
    EXPECT_TRUE( this->compiletime_equal() );
    EXPECT_TRUE( this->runtime_copy_to() );
    EXPECT_TRUE( this->runtime_copy_from() );
    EXPECT_TRUE( this->runtime_equal() );
    EXPECT_TRUE( this->compiletime_dispatch_copy_to() );
    EXPECT_TRUE( this->compiletime_dispatch_copy_from() );
    EXPECT_TRUE( this->compiletime_dispatch_equal() );
    EXPECT_TRUE( this->runtime_dispatch_copy_to() );
    EXPECT_TRUE( this->runtime_dispatch_copy_from() );
    EXPECT_TRUE( this->runtime_dispatch_equal() );
    EXPECT_TRUE( this->equal() );
}

TYPED_TEST_P( PcmIteratorTest, range_based_copy_test )
{
    this->use_range( true );
    this->clear();
    EXPECT_TRUE( this->compiletime_copy_to() );
    EXPECT_TRUE( this->compiletime_copy_from() );
    EXPECT_TRUE( this->compiletime_equal() );
    EXPECT_TRUE( this->runtime_copy_to() );
    EXPECT_TRUE( this->runtime_copy_from() );
    EXPECT_TRUE( this->runtime_equal() );
    EXPECT_TRUE( this->compiletime_dispatch_copy_to() );
    EXPECT_TRUE( this->compiletime_dispatch_copy_from() );
    EXPECT_TRUE( this->compiletime_dispatch_equal() );
    EXPECT_TRUE( this->runtime_dispatch_copy_to() );
    EXPECT_TRUE( this->runtime_dispatch_copy_from() );
    EXPECT_TRUE( this->runtime_dispatch_equal() );
    EXPECT_TRUE( this->equal() );
}

REGISTER_TYPED_TEST_SUITE_P( PcmIteratorTest, iterator_based_copy_test, range_based_copy_test );


template <class Value,
          class Format,
          class SrcContainer = std::vector<Value>,
          class PcmContainer = std::list<uint8_t>,
          class DstContainer = std::vector<Value>>
struct vf
{
    using value_t         = Value;
    using format_t        = Format;
    using src_container_t = SrcContainer;
    using pcm_container_t = PcmContainer;
    using dst_container_t = DstContainer;
};


template <class Value, class Formats>
struct make_iterator_test
{
};

template <class Value, class... Formats>
struct make_iterator_test<Value, std::tuple<Formats...>>
{
    using type = testing::Types<vf<Value, Formats>...>;
};

template <class Value>
using make_iterator_test_t = typename make_iterator_test<Value, pcm::format::tags>::type;
