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

#include <ni/media/pcm/dispatch.h>

#include <algorithm>
#include <utility>

namespace pcm
{
namespace detail
{

struct copy_impl
{
    template <class InputIt, class OutputIt>
    OutputIt operator()( InputIt beg, InputIt end, OutputIt out ) const
    {
        return std::copy( beg, end, out );
    }

    template <class InputIt, class OutputIt>
    auto operator()( InputIt ibeg, InputIt iend, OutputIt obeg, OutputIt oend ) const
    {
        for ( ; ibeg != iend && obeg != oend; ++ibeg, ++obeg )
            *obeg = *ibeg;
        return std::make_pair( ibeg, obeg );
    }
};

} // namespace detail

// iterator based
template <class InputIt, class OutputIt>
auto copy( InputIt beg, InputIt end, OutputIt out )
{
    return dispatch( detail::copy_impl{}, beg, end, out );
}

template <class InputIt, class OutputIt>
auto copy( InputIt ibeg, InputIt iend, OutputIt obeg, OutputIt oend )
{
    return dispatch( detail::copy_impl{}, ibeg, iend, obeg, oend );
}

// range based
template <class InputRange, class OutputIt>
auto copy( const InputRange& range, OutputIt out )
{
    return ::pcm::copy( std::begin( range ), std::end( range ), out );
}

} // namespace pcm
