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


#include "deep_type_replace.h"

#include <ni/media/pcm/detail/find_unique.h>

#include <ni/media/pcm/iterator.h>

#include <algorithm>
#include <assert.h>
#include <tuple>
#include <type_traits>
#include <utility>

namespace pcm
{
namespace detail
{

template <class Format, class Arg>
auto replace( Arg arg )
{
    return deep_type_replace_t<Arg, format, Format>( arg );
}

template <class T1, class T2>
constexpr bool arg_match( const T1& lhs, const T2& rhs )
{
    return lhs == rhs;
}

template <class T>
constexpr bool arg_match( const T&, const T& )
{
    return true;
}

template <class Format, class Arg>
constexpr bool args_match( const Arg& arg )
{
    return arg_match( arg, replace<Format>( arg ) );
}

template <class Format, class Arg0, class... Args>
constexpr bool args_match( const Arg0& arg0, const Args&... args )
{
    return args_match<Format>( arg0 ) && args_match<Format>( args... );
}

template <class Impl, class Format, class... Args>
auto make_impl( Args... args )
{
    using Result              = decltype( std::declval<Impl>()( args... ) );
    Result ( *fp )( Args... ) = []( Args... args ) {
        return static_cast<Result>( Impl{}( replace<Format>( args )... ) );
    };
    return args_match<Format>( args... ) ? fp : nullptr;
}

template <class Impl, class... Formats, class... Args>
auto get_dispatched_impl( const std::tuple<Formats...>&, Args... args )
{
    const auto impls = {make_impl<Impl, Formats>( args... )...};
    const auto iter  = find_unique_if( impls, []( auto ptr ) { return ptr != nullptr; } );

    return iter != impls.end() ? *iter : nullptr;
}

struct static_t
{
};
struct dynamic_t
{
};

template <class... Args>
using dispatch_t = std::conditional_t<
    std::is_same<std::tuple<Args...>, deep_type_replace_t<std::tuple<Args...>, format, compiletime_format<>>>::value,
    static_t,
    dynamic_t>;

template <class Impl, class... Args>
auto dispatch( static_t, Impl&& impl, Args... args )
{
    return std::forward<Impl>( impl )( args... );
}

template <class Impl, class... Args>
auto dispatch( dynamic_t, Impl&& impl, Args... args )
{
    auto dispatched_impl = get_dispatched_impl<std::decay_t<Impl>>( format::tags{}, args... );
    assert( dispatched_impl );

    return dispatched_impl ? ( *dispatched_impl )( args... ) : std::forward<Impl>( impl )( args... );
}

template <class Impl, class... Args>
auto dispatch( Impl&& impl, Args... args )
{
    return dispatch( dispatch_t<Args...>{}, std::forward<Impl>( impl ), args... );
}

} // namespace detail
} // namespace pcm
