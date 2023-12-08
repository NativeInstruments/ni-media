//
// Copyright (c) 2017-2023 Native Instruments GmbH, Berlin
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

#include <boost/optional.hpp>

#include <cstdint>
#include <map>

namespace audio
{

class wav_specific_info
{
public:
    struct InstrumentChunk
    {
        uint8_t root_note;
        int8_t  fine_tune;
        int8_t  gain;
        uint8_t low_note;
        uint8_t high_note;
        uint8_t low_velocity;
        uint8_t high_velocity;
    };

    struct SampleChunk
    {
        int32_t manufacturer;
        int32_t product;
        int32_t sample_period;
        int32_t midi_unity_note;
        int32_t midi_pitch_fraction;
        int32_t smpte_format;
        int32_t smpte_offset;
        int32_t sample_loops;
        int32_t sampler_data;
    };

    struct SampleLoop
    {
        int32_t type;
        int32_t start;
        int32_t end;
        int32_t fraction;
        int32_t play_count;
    };

    using SampleLoops = std::map<int32_t /*id*/, SampleLoop>;

    auto const& instrument_chunk() const
    {
        return m_instrument_chunk;
    }

    void instrument_chunk( InstrumentChunk ic )
    {
        m_instrument_chunk = ic;
    }

    auto const& sample_chunk() const
    {
        return m_sample_chunk;
    }

    void sample_chunk( SampleChunk sc )
    {
        m_sample_chunk = sc;
    }

    auto const& sample_loops() const
    {
        return m_sample_loops;
    }

    void sample_loops( SampleLoops sl )
    {
        m_sample_loops = sl;
    }

private:
    boost::optional<InstrumentChunk> m_instrument_chunk = boost::none;
    boost::optional<SampleChunk>     m_sample_chunk     = boost::none;
    boost::optional<SampleLoops>     m_sample_loops     = boost::none;
};

} // namespace audio
