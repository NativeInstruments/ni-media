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

#include <ni/media/audio/ifstream_info.h>
#include <ni/media/audio/wav/wav_specific_info.h>


namespace audio
{

class wav_ifstream_info : public ifstream_info, public wav_specific_info
{
public:
    wav_ifstream_info( const wav_ifstream_info& ) = default;

    wav_ifstream_info( const ifstream_info& ifstream_info_ = {}, const wav_specific_info& wav_specific_info_ = {} )
    : ifstream_info( ifstream_info_ )
    , wav_specific_info( wav_specific_info_ )
    {
        lossless( true );
        container( container_type::wav );
        codec( codec_type::wav );
    }
};

} // namespace audio
