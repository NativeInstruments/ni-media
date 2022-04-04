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

#include <ni/media/pcm/format.h>

#include <memory>
#include <type_traits>
#include <utility>

namespace audio
{

class stream_info
{

public:
    virtual ~stream_info() = default;

    using format_type = ::pcm::format;

    void format( format_type format );
    auto format() const -> format_type;

    void   sample_rate( size_t val );
    size_t sample_rate() const;

    void   num_channels( size_t num_channels );
    size_t num_channels() const;

    size_t bits_per_sample() const;
    size_t bytes_per_sample() const;
    size_t bytes_per_frame() const;

    // Bitrate of how the underlying data is encoded/decoded.
    void bit_rate( size_t bit_rate );
    size_t bit_rate() const;

    friend bool operator==( const stream_info& lhs, const stream_info& rhs );
    friend bool operator!=( const stream_info& lhs, const stream_info& rhs );

private:
    size_t compute_bit_rate() const;

    // Members
    size_t      m_sample_rate  = 44100;
    size_t      m_num_channels = 1;
    size_t      m_bit_rate = 0;
    format_type m_format;
};

//----------------------------------------------------------------------------------------------------------------------

template <typename Info>
auto make_info( Info&& info )
{
    return std::make_unique<std::decay_t<Info>>( std::forward<Info>( info ) );
}

} // namespace audio


//----------------------------------------------------------------------------------------------------------------------
