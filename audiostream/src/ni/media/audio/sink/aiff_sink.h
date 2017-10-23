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

#include <ni/media/audio/aiff/aiff_chunks.h>
#include <ni/media/audio/aiff/aiff_ofstream_info.h>
#include <ni/media/iostreams/device/subview.h>
#include <ni/media/iostreams/write_obj.h>


namespace detail
{

template <class Sink>
auto write_aiff_header( Sink& sink )
{
}

template <class Sink>
auto close_aiff( Sink& sink )
{
}

} // namespace detail

template <class Sink>
class aiff_sink : public boostext::iostreams::subview_sink<Sink>
{
    using base_type   = boostext::iostreams::subview_sink<Sink>;
    using offset_type = typename base_type::offset_type;

public:
    using info_type = audio::aiff_ofstream_info;

    template <class... Args>
    explicit aiff_sink( info_type info, Args&&... args )
    : base_type( std::forward<Args>( args )... )
    , m_info( info )
    {
        detail::write_aiff_header( *this );
        auto pos = this->tell();
        this->set_view( pos );
    }

    void close()
    {
        this->set_view( 0 );
        detail::close_aiff( *this );
        base_type::close();
        m_info = info_type();
    }

    auto info() const -> info_type
    {
        return m_info;
    }

private:
    info_type m_info;
};
