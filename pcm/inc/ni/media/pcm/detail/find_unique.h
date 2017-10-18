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

#include <algorithm>
#include <iterator>

namespace pcm
{
namespace detail
{

template <class InputIt, class Value>
auto find_unique( InputIt beg, InputIt end, const Value& value )
{
    const auto it     = std::find( beg, end, value );
    const bool unique = end != it && end == std::find( std::next( it ), end, value );
    return unique ? it : end;
}

template <class InputRange, class Value>
auto find_unique( const InputRange& range, const Value& value )
{
    return find_unique( std::begin( range ), std::end( range ), value );
}

template <class InputIt, class UnaryPredicate>
auto find_unique_if( InputIt beg, InputIt end, UnaryPredicate pred )
{
    const auto it     = std::find_if( beg, end, pred );
    const bool unique = end != it && end == std::find_if( std::next( it ), end, pred );
    return unique ? it : end;
}

template <class InputRange, class UnaryPredicate>
auto find_unique_if( const InputRange& range, UnaryPredicate pred )
{
    return find_unique_if( std::begin( range ), std::end( range ), pred );
}

} // namespace detail
} // namespace pcm
