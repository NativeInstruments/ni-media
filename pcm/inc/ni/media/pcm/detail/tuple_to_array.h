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

#include <boost/core/ignore_unused.hpp>

#include <array>
#include <tuple>
#include <utility>


namespace pcm
{
namespace detail
{

template <typename Type, typename Tuple, std::size_t... Indices>
auto tuple_to_array_impl( Tuple&& tuple, std::index_sequence<Indices...> ) -> std::array<Type, sizeof...( Indices )>
{
    boost::ignore_unused( tuple );
    return {{static_cast<Type>( std::get<Indices>( std::forward<Tuple>( tuple ) ) )...}};
}

template <typename Type, typename Tuple>
auto tuple_to_array( Tuple&& tuple )
{
    boost::ignore_unused( tuple );
    return tuple_to_array_impl<Type>( std::forward<Tuple>( tuple ),
                                      std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>() );
}

} // namespace detail
} // namespace pcm
