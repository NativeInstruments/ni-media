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

#include <boost/math/constants/constants.hpp>
#include <cmath>


//----------------------------------------------------------------------------------------------------------------------

struct sin440_mod
{
    using result_type = float;

    sin440_mod( size_t num_channels, float sample_rate, int multiplier )
    : m_freq( 440.f * boost::math::constants::two_pi<float>() / sample_rate )
    , m_mod_freq( multiplier * 23.f * boost::math::constants::two_pi<float>() / sample_rate )
    , m_mod_amp( multiplier * 16.f )
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
    const float m_freq;
    const float m_mod_freq;
    const float m_mod_amp;
    size_t      m_num_channels;
    size_t      m_current_sample = 0;
};
