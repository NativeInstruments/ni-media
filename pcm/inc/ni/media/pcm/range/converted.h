//
// Copyright (c) 2017-2023 Native Instruments GmbH, Berlin
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

#include <ni/media/pcm/iterator.h>

#include <boost/range/iterator_range.hpp>


namespace pcm
{
namespace converted_detail
{

template <class Value, class Format>
struct format_holder
{
    Format fmt;
    using value_type = Value;
};

template <class Rng, class Value, class Format>
auto operator|( Rng& r, const format_holder<Value, Format>& h )
{
    return boost::make_iterator_range(
        pcm::make_iterator<typename format_holder<Value, Format>::value_type>( std::begin( r ), h.fmt ),
        pcm::make_iterator<typename format_holder<Value, Format>::value_type>( std::end( r ), h.fmt ) );
}

template <class Rng, class Value, class Format>
auto operator|( const Rng& r, const format_holder<Value, Format>& h )
{
    return boost::make_iterator_range(
        pcm::make_iterator<typename format_holder<Value, Format>::value_type>( std::begin( r ), h.fmt ),
        pcm::make_iterator<typename format_holder<Value, Format>::value_type>( std::end( r ), h.fmt ) );
}

} // namespace converted_detail


template <class Value, class Format>
auto converted_to( const Format& from ) -> converted_detail::format_holder<Value, Format>
{
    return {from};
}


} // namespace pcm
