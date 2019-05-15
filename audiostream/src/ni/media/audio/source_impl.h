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

#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/positioning.hpp>

#include <memory>

template <class Impl>
class source_impl
{

public:
    using char_type = char;
    struct category : boost::iostreams::input_seekable, boost::iostreams::device_tag
    {
    };

    using info_type = typename Impl::info_type;

    source_impl()  = default;
    ~source_impl() = default;

    source_impl( source_impl&& ) = default;

    template <class... Args>
    source_impl( Args&&... args )
    : m_impl( std::make_unique<Impl>( std::forward<Args>( args )... ) )
    {
    }

    auto seek( boost::iostreams::stream_offset offset, BOOST_IOS::seekdir way = BOOST_IOS::beg ) -> std::streampos
    {
        return m_impl->seek( offset, way );
    }

    auto read( char_type* dst, std::streamsize size ) -> std::streamsize
    {
        return m_impl->read( dst, size );
    }

    auto info() const -> info_type
    {
        return m_impl->info();
    }

private:
    std::unique_ptr<Impl> m_impl;
};
