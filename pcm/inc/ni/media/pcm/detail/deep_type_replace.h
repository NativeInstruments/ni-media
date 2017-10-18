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

namespace pcm
{
namespace detail
{

template <class T, class From, class To>
struct list_replace;

template <class T, class From, class To>
struct deep_type_replace
{
    using type = typename list_replace<T, From, To>::type;
};

template <class From, class To>
struct deep_type_replace<From, From, To>
{
    using type = To;
};

template <class T, class From, class To>
using deep_type_replace_t = typename deep_type_replace<T, From, To>::type;


template <class T, class From, class To>
struct list_replace
{
    using type = T;
};

template <template <typename...> class List, class... Ts, class From, class To>
struct list_replace<List<Ts...>, From, To>
{
    using type = List<deep_type_replace_t<Ts, From, To>...>;
};

template <template <typename...> class List, class... Ts, class From, class To>
struct list_replace<List<Ts...>&, From, To>
{
    using type = List<deep_type_replace_t<Ts, From, To>...>&;
};

template <template <typename...> class List, class... Ts, class From, class To>
struct list_replace<List<Ts...>&&, From, To>
{
    using type = List<deep_type_replace_t<Ts, From, To>...>&&;
};

template <template <typename...> class List, class... Ts, class From, class To>
struct list_replace<const List<Ts...>, From, To>
{
    using type = const List<deep_type_replace_t<Ts, From, To>...>;
};

template <template <typename...> class List, class... Ts, class From, class To>
struct list_replace<const List<Ts...>&, From, To>
{
    using type = const List<deep_type_replace_t<Ts, From, To>...>&;
};

template <template <typename...> class List, class... Ts, class From, class To>
struct list_replace<const List<Ts...>&&, From, To>
{
    using type = const List<deep_type_replace_t<Ts, From, To>...>&&;
};

template <template <typename...> class List, class... Ts, class From, class To>
struct list_replace<volatile List<Ts...>, From, To>
{
    using type = volatile List<deep_type_replace_t<Ts, From, To>...>;
};

template <template <typename...> class List, class... Ts, class From, class To>
struct list_replace<volatile List<Ts...>&, From, To>
{
    using type = volatile List<deep_type_replace_t<Ts, From, To>...>&;
};

template <template <typename...> class List, class... Ts, class From, class To>
struct list_replace<volatile List<Ts...>&&, From, To>
{
    using type = volatile List<deep_type_replace_t<Ts, From, To>...>&&;
};

template <template <typename...> class List, class... Ts, class From, class To>
struct list_replace<const volatile List<Ts...>, From, To>
{
    using type = const volatile List<deep_type_replace_t<Ts, From, To>...>;
};

template <template <typename...> class List, class... Ts, class From, class To>
struct list_replace<const volatile List<Ts...>&, From, To>
{
    using type = const volatile List<deep_type_replace_t<Ts, From, To>...>&;
};

template <template <typename...> class List, class... Ts, class From, class To>
struct list_replace<const volatile List<Ts...>&&, From, To>
{
    using type = const volatile List<deep_type_replace_t<Ts, From, To>...>&&;
};

} // namespace detail
} // namespace pcm
