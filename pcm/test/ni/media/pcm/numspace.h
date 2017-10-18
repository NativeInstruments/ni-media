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

#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/irange.hpp>

#include <ni/media/pcm/limits.h>

namespace pcm
{

namespace detail
{

template <typename Value>
struct linear_map
{
    Value scale;
    Value offset;

    template <typename OtherValue>
    auto operator()( OtherValue num ) const
    {
        return Value( scale * num + offset );
    }
};

} // namespace detail


template <typename Value>
auto numspace( int bits = numeric_limits<Value>::bits() )
{
    const auto scale  = Value( numeric_limits<Value>::step( bits ) );
    const auto offset = Value( numeric_limits<Value>::min() );

    return boost::irange( size_t( 0 ), size_t( 1 ) << bits )
           | boost::adaptors::transformed( detail::linear_map<Value>{scale, offset} );
}

} // namespace pcm
