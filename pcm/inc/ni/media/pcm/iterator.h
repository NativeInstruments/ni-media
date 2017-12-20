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

#include "converter.h"
#include "format.h"

#include <boost/iterator/iterator_facade.hpp>
#include <iterator> // std::iterator_traits
#include <type_traits>

#include <boost/core/ignore_unused.hpp>

namespace pcm
{

namespace detail
{

template <class Value, class Iterator, class Format>
struct proxy : Format
{
    proxy() = default;

    proxy& operator=( Value val )
    {
        write( iter, val, format() );
        return *this;
    }

    operator Value() const
    {
        return read<Value>( iter, format() );
    }

private:
    template <class, class, class, class>
    friend class iterator;
    template <class, class, class>
    friend struct proxy;

    template <class OtherIterator,
              class OtherFormat,
              class = std::enable_if_t<std::is_convertible<OtherIterator, Iterator>::value
                                       && std::is_convertible<OtherFormat, Format>::value>>
    proxy( const proxy<Value, OtherIterator, OtherFormat>& other )
    : Format( other.format() )
    , iter( other.iter )
    {
    }

    proxy( const Format& fmt, Iterator it )
    : Format( fmt )
    , iter( it )
    {
    }

    auto step() const -> typename std::iterator_traits<Iterator>::difference_type
    {
        return format().bitwidth() / 8;
    }

    const Format& format() const
    {
        return static_cast<const Format&>( *this );
    }

    Iterator iter;
};


template <class T>
using is_const_reference =
    typename std::integral_constant<bool,
                                    std::is_reference<T>::value
                                        && std::is_const<typename std::remove_reference<T>::type>::value>::type;

template <class T>
using is_mutable_reference =
    typename std::integral_constant<bool,
                                    std::is_reference<T>::value
                                        && !std::is_const<typename std::remove_reference<T>::type>::value>::type;

template <class T>
using is_const_iterator = is_const_reference<typename std::iterator_traits<T>::reference>;

template <class T>
using is_mutable_iterator = is_mutable_reference<typename std::iterator_traits<T>::reference>;


template <class Value, class Iterator, class Format>
using iterator_reference_t =
    typename std::conditional<is_const_iterator<Iterator>::value, Value, proxy<Value, Iterator, Format>>::type;

template <typename Value,
          typename Iterator,
          typename Format,
          typename Category = typename std::iterator_traits<Iterator>::iterator_category>
class iterator : public boost::iterator_facade<iterator<Value, Iterator, Format, Category>,
                                               Value,
                                               Category,
                                               iterator_reference_t<Value, Iterator, Format>>
{
    static_assert( std::is_base_of<std::forward_iterator_tag, Category>::value, "Unsupported iterator category" );

    using base_t = boost::iterator_facade<iterator<Value, Iterator, Format, Category>,
                                          Value,
                                          Category,
                                          iterator_reference_t<Value, Iterator, Format>>;

    using proxy_t = proxy<Value, Iterator, Format>;
    using this_t  = iterator<Value, Iterator, Format, Category>;

    template <class, class, class, class>
    friend class iterator;

public:
    using value_type      = typename base_t::value_type;
    using difference_type = typename base_t::difference_type;
    using reference       = typename base_t::reference;

    iterator() = default;

    template <class OtherIterator,
              class OtherFormat,
              class = std::enable_if_t<std::is_convertible<OtherIterator, Iterator>::value
                                       && std::is_convertible<OtherFormat, Format>::value>>
    iterator( const iterator<Value, OtherIterator, OtherFormat>& other )
    : m_proxy( other.m_proxy )
    {
    }


    iterator( Iterator iter, const Format& fmt )
    : m_proxy( fmt, iter )
    {
    }

    const Iterator& base() const
    {
        return m_proxy.iter;
    }

    const Format& format() const
    {
        return m_proxy.format();
    }

private:
    void increment()
    {
        std::advance( m_proxy.iter, m_proxy.step() );
    }

    void decrement()
    {
        std::advance( m_proxy.iter, -m_proxy.step() );
    }

    void advance( difference_type offset )
    {
        std::advance( m_proxy.iter, offset * m_proxy.step() );
    }

    difference_type distance_to( const iterator& other ) const
    {
        return std::distance( m_proxy.iter, other.m_proxy.iter ) / m_proxy.step();
    }

    template <class OtherIterator,
              class OtherFormat,
              class = std::enable_if_t<std::is_convertible<OtherIterator, Iterator>::value
                                       && std::is_convertible<OtherFormat, Format>::value>>
    bool equal( const iterator<Value, OtherIterator, OtherFormat>& other ) const
    {
        assert( m_proxy.format() == other.m_proxy.format() );
        return m_proxy.iter == other.m_proxy.iter;
    }

    reference dereference() const
    {
        return m_proxy;
    }

    friend class ::boost::iterator_core_access;

    proxy_t m_proxy;
};


template <typename Value, typename Iterator, typename Format>
class iterator<Value, Iterator, Format, std::input_iterator_tag>
: public boost::iterator_facade<iterator<Value, Iterator, Format, std::input_iterator_tag>,
                                Value,
                                std::input_iterator_tag,
                                const Value&>
{
    using base_t = boost::iterator_facade<iterator<Value, Iterator, Format, std::input_iterator_tag>,
                                          Value,
                                          std::input_iterator_tag,
                                          const Value&>;

    using proxy_t = proxy<Value, Iterator, Format>;
    using this_t  = iterator<Value, Iterator, Format, std::input_iterator_tag>;

    template <class, class, class, class>
    friend class iterator;

public:
    using value_type      = typename base_t::value_type;
    using difference_type = typename base_t::difference_type;
    using reference       = typename base_t::reference;

    iterator() = default;

    template <class OtherIterator,
              class OtherFormat,
              class = std::enable_if_t<std::is_convertible<OtherIterator, Iterator>::value
                                       && std::is_convertible<OtherFormat, Format>::value>>
    iterator( const iterator<Value, OtherIterator, OtherFormat, std::input_iterator_tag>& other )
    : m_proxy( other.m_proxy )
    , m_value( other.m_value )
    {
    }

    iterator( Iterator iter, const Format& fmt )
    : m_proxy( fmt, iter )
    {
        if ( iter != Iterator{} )
            increment();
    }

    const Iterator& base() const
    {
        return m_proxy.iter;
    }

    const Format& format() const
    {
        return m_proxy.format();
    }

private:
    void increment()
    {
        m_value = m_proxy;
    }

    template <class OtherIterator,
              class OtherFormat,
              class = std::enable_if_t<std::is_convertible<OtherIterator, Iterator>::value
                                       && std::is_convertible<OtherFormat, Format>::value>>
    bool equal( const iterator<Value, OtherIterator, OtherFormat, std::input_iterator_tag>& other ) const
    {
        assert( m_proxy.format() == other.m_proxy.format() );
        return m_proxy.iter == other.m_proxy.iter;
    }

    reference dereference() const
    {
        return m_value;
    }

    friend class ::boost::iterator_core_access;

    proxy_t m_proxy;
    Value   m_value;
};

} // namespace detail

template <typename Value, typename Iterator, typename Format = ::pcm::format>
using iterator = detail::iterator<Value, Iterator, Format>;


template <typename Value, typename Iterator, typename Format>
auto make_iterator( Iterator it, Format f ) -> iterator<Value, Iterator, Format>
{
    return {it, f};
}

} // namespace pcm
