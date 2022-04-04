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

#include <ni/media/audio/stream_info.h>

#include <tuple>

namespace audio
{

//----------------------------------------------------------------------------------------------------------------------

void stream_info::format( format_type format )
{
    m_format = format;
}

//----------------------------------------------------------------------------------------------------------------------

auto stream_info::format() const -> format_type
{
    return m_format;
}

//----------------------------------------------------------------------------------------------------------------------

void stream_info::sample_rate( size_t val )
{
    m_sample_rate = val;
}

//----------------------------------------------------------------------------------------------------------------------

size_t stream_info::sample_rate() const
{
    return m_sample_rate;
}

//----------------------------------------------------------------------------------------------------------------------

void stream_info::num_channels( size_t num_channels )
{
    m_num_channels = num_channels;
}

//----------------------------------------------------------------------------------------------------------------------

size_t stream_info::num_channels() const
{
    return m_num_channels;
}

//----------------------------------------------------------------------------------------------------------------------

size_t stream_info::bits_per_sample() const
{
    return m_format.bitwidth();
}

//----------------------------------------------------------------------------------------------------------------------

size_t stream_info::bytes_per_sample() const
{
    return bits_per_sample() / 8;
}

//----------------------------------------------------------------------------------------------------------------------

size_t stream_info::bytes_per_frame() const
{
    return bytes_per_sample() * num_channels();
}

//----------------------------------------------------------------------------------------------------------------------

void stream_info::bit_rate( size_t bit_rate )
{
    m_bit_rate = bit_rate;
}

//----------------------------------------------------------------------------------------------------------------------

size_t stream_info::bit_rate() const
{
    return m_bit_rate == 0 ? compute_bit_rate() : m_bit_rate;
}

//----------------------------------------------------------------------------------------------------------------------

size_t stream_info::compute_bit_rate() const
{
    return sample_rate() * num_channels() * bits_per_sample();
}

//----------------------------------------------------------------------------------------------------------------------

bool operator==( const stream_info& lhs, const stream_info& rhs )
{
    return std::tie( lhs.m_sample_rate, lhs.m_num_channels, lhs.m_format )
           == std::tie( rhs.m_sample_rate, rhs.m_num_channels, rhs.m_format );
}

//----------------------------------------------------------------------------------------------------------------------

bool operator!=( const stream_info& lhs, const stream_info& rhs )
{
    return !( lhs == rhs );
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace audio
