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

#include <ni/media/audio/fstream_info.h>
#include <ni/media/audio/istream_info.h>

namespace audio
{

class ifstream_info : public fstream_info, public istream_info
{
public:
    enum class container_type
    {
        aiff,
        flac,
        mp3,
        mp4,
        ogg,
        wav,
        wma
    };

    enum class codec_type
    {
        aac,
        alac,
        aiff,
        flac,
        mp3,
        vorbis,
        wav,
        wma
    };

    void num_frames( size_t num_frames );
    size_t num_frames() const;

    size_t num_samples() const;
    size_t num_bytes() const;

    void codec( codec_type value );
    auto codec() const -> codec_type;

    void container( container_type value );
    auto container() const -> container_type;

    friend bool operator==( const ifstream_info& lhs, const ifstream_info& rhs );
    friend bool operator!=( const ifstream_info& lhs, const ifstream_info& rhs );

private:
    size_t         m_num_frames = 0;
    container_type m_container;
    codec_type     m_codec;
};

} // namespace audio


//----------------------------------------------------------------------------------------------------------------------
