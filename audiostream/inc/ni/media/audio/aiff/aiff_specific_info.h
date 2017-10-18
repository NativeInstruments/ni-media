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

#include <boost/optional.hpp>

#include <cstdint>
#include <map>

namespace audio
{

class aiff_specific_info
{
public:
    using MarkerMap = std::map<int16_t /*id*/, uint32_t /*position*/>;

    struct Loop
    {
        int16_t play_mode;
        int16_t marker_id_begin;
        int16_t marker_id_end;
    };

    struct InstrumentChunk
    {
        uint8_t root_note;
        uint8_t detune;
        uint8_t low_note;
        uint8_t high_note;
        uint8_t low_velocity;
        uint8_t high_velocity;
        int16_t gain;
        Loop    sustain_loop;
        Loop    release_loop;
    };

    auto const& marker_map() const
    {
        return m_marker_map;
    }

    void marker_map( MarkerMap marker_map )
    {
        m_marker_map = marker_map;
    }

    auto const& instrument_chunk() const
    {
        return m_instrument_chunk;
    }

    void instrument_chunk( InstrumentChunk instrument_chunk )
    {
        m_instrument_chunk = instrument_chunk;
    }

private:
    MarkerMap                        m_marker_map;
    boost::optional<InstrumentChunk> m_instrument_chunk = boost::none;
};

} // namespace audio
