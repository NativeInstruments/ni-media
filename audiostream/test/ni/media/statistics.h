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

#include <boost/range/combine.hpp>

#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/max_element.hpp>
#include <boost/range/numeric.hpp>
#include <boost/range/value_type.hpp>

#include <cmath>


//----------------------------------------------------------------------------------------------------------------------

template <typename Range1, typename Range2>
auto xcorr( Range1 const& r1, Range2 const& r2 ) -> double
{
    using value_t = typename boost::range_value<Range1>::type;

    auto mean_r1 = boost::accumulate( r1, value_t{} ) / r1.size();
    auto mean_r2 = boost::accumulate( r2, value_t{} ) / r1.size();

    auto numerator = boost::inner_product( r1, r2, value_t{} ) - r1.size() * mean_r1 * mean_r2;

    auto d1 = boost::accumulate(
        r1, value_t{}, [mean_r1]( value_t init, value_t v ) { return init + ( v - mean_r1 ) * ( v - mean_r1 ); } );
    auto d2 = boost::accumulate(
        r2, value_t{}, [mean_r2]( value_t init, value_t v ) { return init + ( v - mean_r2 ) * ( v - mean_r2 ); } );

    return numerator / std::sqrt( d1 * d2 );
}

//----------------------------------------------------------------------------------------------------------------------

template <typename Range1, typename Range2>
auto dc_offset( Range1 const& r1, Range2 const& r2 ) -> double
{
    using value_t = typename boost::range_value<Range1>::type;

    double mean_r1 = boost::accumulate( r1, value_t{} ) / r1.size();
    double mean_r2 = boost::accumulate( r2, value_t{} ) / r2.size();

    return std::abs( mean_r1 - mean_r2 );
}

//----------------------------------------------------------------------------------------------------------------------

template <typename Range1, typename Range2>
auto max_difference( Range1 const& r1, Range2 const& r2 )
{
    auto diff       = []( const auto& x ) { return std::abs( boost::get<0>( x ) - boost::get<1>( x ) ); };
    auto diff_range = boost::combine( r1, r2 ) | boost::adaptors::transformed( std::ref( diff ) );

    auto it = boost::max_element( diff_range );
    return it == std::end( diff_range ) ? decltype( *it ){} : *it;
}
