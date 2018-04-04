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

#include <ni/media/audio/aiff/aiff_specific_info.h>
#include <ni/media/audio/ofstream_info.h>

namespace audio
{

class aiff_ofstream_info : public ofstream_info, public aiff_specific_info
{
public:
    aiff_ofstream_info( const aiff_ofstream_info& ) = default;

    aiff_ofstream_info( const ofstream_info& ofstream_info_ = {}, const aiff_specific_info& aiff_specific_info_ = {} )
    : ofstream_info( ofstream_info_ )
    , aiff_specific_info( aiff_specific_info_ )
    {
        lossless( true );
        container( container_type::aiff );
        codec( codec_type::aiff );
    }
};

} // namespace audio
