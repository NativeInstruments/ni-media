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

#include <cstddef>
#include <tuple>
#include <type_traits>

namespace pcm
{
namespace detail
{

template <class T, class Tuple>
struct tuple_find
{
    using fail = std::integral_constant<bool, !std::is_same<T, T>::value>;
    static_assert( fail::value, "Not a tuple" );
};

template <class T>
struct tuple_find<T, std::tuple<>> : public std::integral_constant<size_t, 0>
{
}; // not found

template <class T, class... Types>
struct tuple_find<T, std::tuple<T, Types...>> : public std::integral_constant<size_t, 0>
{
}; // found

template <class T, class U, class... Us>
struct tuple_find<T, std::tuple<U, Us...>>
: public std::integral_constant<size_t, 1 + tuple_find<T, std::tuple<Us...>>::value>
{
};

} // namespace detail
} // namespace pcm
