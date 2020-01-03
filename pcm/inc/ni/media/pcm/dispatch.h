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

#include <ni/media/pcm/iterator.h>

#include <algorithm>
#include <cassert>
#include <tuple>
#include <type_traits>
#include <utility>

namespace pcm
{
namespace detail
{

struct catch_t
{
};

struct try_t : catch_t
{
};


template <class Result, class F, class... Ts>
auto invoke( try_t, F f, Ts... ts ) -> decltype( f( ts... ), Result() )
{
    return f( ts... );
}


template <class Result, class F, class... Ts>
Result invoke( catch_t, F, Ts... )
{
    throw std::runtime_error( "runtime argument mismatch" );
    // ending up here means you are calling dispatch with incompatible runtime formats.
    // i.e. the code below will throw a runtime error:
    // pcm::copy( pcm::make_iterator<float>( in.begin(), pcm::runtime_format( pcm::signed_integer, pcm::_24bit ) ),
    //            pcm::make_iterator<float>( in.end(), pcm::runtime_format( pcm::signed_integer, pcm::_16bit ) ),
    //            out.begin() )
    //
    // whereas the same code using a compiletime format would result in a compiletime error:
    // pcm::copy( pcm::make_iterator<float>( in.begin(), pcm::compiletime_format< pcm::signed_integer, pcm::_24bit>() ),
    //            pcm::make_iterator<float>( in.end(), pcm::compiletime_format< pcm::signed_integer, pcm::_16bit>() ),
    //            out.begin() )
}


template <class...>
struct dispatch_impl
{
};

// Base case
template <class R, class F, class... Bs, class... Formats>
struct dispatch_impl<R, F, std::tuple<>, std::tuple<Bs...>, std::tuple<Formats...>>
{
    auto operator()( F f, Bs... bs )
    {
        return invoke<R>( try_t{}, f, bs... );
    }
};

// Recursive case without dispatch
template <class R, class F, class A, class... As, class... Bs, class... Formats>
struct dispatch_impl<R, F, std::tuple<A, As...>, std::tuple<Bs...>, std::tuple<Formats...>>
{
    auto operator()( F f, A a, As... as, Bs... bs )
    {
        auto impl = dispatch_impl<R, F, std::tuple<As...>, std::tuple<Bs..., A>, std::tuple<Formats...>>();
        return impl( f, as..., bs..., a );
    }
};


// Recursive case, with dispatch, with return type
template <class R, class F, class Value, class Iter, class... As, class... Bs, class... Formats>
struct dispatch_impl<R, F, std::tuple<::pcm::iterator<Value, Iter>, As...>, std::tuple<Bs...>, std::tuple<Formats...>>
{
    auto operator()( F f, ::pcm::iterator<Value, Iter> it, As... as, Bs... bs )
    {
        R r;
        []( ... ) {}( it.format() == Formats{}
                      && ( r = ( dispatch_impl<R,
                                               F,
                                               std::tuple<As...>,
                                               std::tuple<Bs..., ::pcm::iterator<Value, Iter, Formats>>,
                                               std::tuple<Formats...>>()(
                               f, as..., bs..., ::pcm::iterator<Value, Iter, Formats>( it.base(), Formats{} ) ) ),
                           false )... );
        return r;
    }
};

// Recursive case, with dispatch, without return type
template <class F, class Value, class Iter, class... As, class... Bs, class... Formats>
struct dispatch_impl<void, F, std::tuple<::pcm::iterator<Value, Iter>, As...>, std::tuple<Bs...>, std::tuple<Formats...>>
{
    auto operator()( F f, ::pcm::iterator<Value, Iter> it, As... as, Bs... bs )
    {
        []( ... ) {}( it.format() == Formats{}
                      && ( dispatch_impl<void,
                                         F,
                                         std::tuple<As...>,
                                         std::tuple<Bs..., ::pcm::iterator<Value, Iter, Formats>>,
                                         std::tuple<Formats...>>()(
                               f, as..., bs..., ::pcm::iterator<Value, Iter, Formats>( it.base(), Formats{} ) ),
                           false )... );
    }
};

} // namespace detail

template <class F, class... Ts>
auto dispatch( F f, Ts... ts )
{
    using Result  = decltype( f( ts... ) );
    using Formats = decltype( compiletime_formats() );
    auto impl     = detail::dispatch_impl<Result, F, std::tuple<Ts...>, std::tuple<>, Formats>();
    return impl( f, ts... );
}

} // namespace pcm
