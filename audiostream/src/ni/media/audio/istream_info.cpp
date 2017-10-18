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

#include <ni/media/audio/istream_info.h>

#include <tuple>

namespace audio
{

//----------------------------------------------------------------------------------------------------------------------

void istream_info::num_frames( size_t num_frames )
{
    m_numFrames = num_frames;
}

//----------------------------------------------------------------------------------------------------------------------

size_t istream_info::num_frames() const
{
    return m_numFrames;
}

//----------------------------------------------------------------------------------------------------------------------

size_t istream_info::num_samples() const
{
    return num_frames() * num_channels();
}

//----------------------------------------------------------------------------------------------------------------------

size_t istream_info::num_bytes() const
{
    return num_samples() * bytes_per_sample();
}

//----------------------------------------------------------------------------------------------------------------------

bool operator==( const istream_info& lhs, const istream_info& rhs )
{
    return static_cast<const stream_info&>( lhs ) == static_cast<const stream_info&>( rhs )
           && lhs.m_numFrames == rhs.m_numFrames;
}

//----------------------------------------------------------------------------------------------------------------------

bool operator!=( const istream_info& lhs, const istream_info& rhs )
{
    return !( lhs == rhs );
}

} // namespace audio
