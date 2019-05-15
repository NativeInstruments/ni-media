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

#include <boost/math/constants/constants.hpp>
#include <cmath>

//----------------------------------------------------------------------------------------------------------------------
template <class Value>
struct modulated_sine
{
    using result_type = Value;

    modulated_sine( size_t num_channels, size_t sample_rate, Value freq, Value mod_freq = 0, Value mod_amp = 0 )
    : m_freq( freq * boost::math::constants::two_pi<Value>() / sample_rate )
    , m_mod_freq( mod_freq * boost::math::constants::two_pi<Value>() / sample_rate )
    , m_mod_amp( mod_amp )
    , m_num_channels( num_channels )
    {
    }

    result_type operator()()
    {
        const auto frame   = m_current_sample / m_num_channels;
        const auto channel = m_current_sample % m_num_channels;
        ++m_current_sample;

        return channel == 0 ? sin( m_freq * frame + m_mod_amp * sin( m_mod_freq * frame ) )
                            : cos( m_freq * frame + m_mod_amp * sin( m_mod_freq * frame ) );
    }

private:
    const result_type m_freq;
    const result_type m_mod_freq;
    const result_type m_mod_amp;
    size_t            m_num_channels;
    size_t            m_current_sample = 0;
};
