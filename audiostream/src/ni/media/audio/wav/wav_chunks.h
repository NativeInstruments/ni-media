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

#include <cstdint>


//----------------------------------------------------------------------------------------------------------------------
// WAV Chunks
//----------------------------------------------------------------------------------------------------------------------

namespace wav
{

#pragma pack( push, 1 )

struct RiffTag
{
    uint32_t id     = 0;
    uint32_t length = 0;
};


//----------------------------------------------------------------------------------------------------------------------

struct FmtChunk
{
    uint16_t formatTag      = 0;
    uint16_t numChannels    = 0;
    uint32_t sampleRate     = 0;
    uint32_t avgBytesPerSec = 0;
    uint16_t blockAlign     = 0;
    uint16_t bitsPerSample  = 0;
};
static_assert( sizeof( FmtChunk ) == 16, "sizeof(FmtChunk) is wrong" );

//----------------------------------------------------------------------------------------------------------------------

struct SampleLoop
{
    int32_t identifier = 0;
    int32_t type       = 0;
    int32_t start      = 0;
    int32_t end        = 0;
    int32_t fraction   = 0;
    int32_t playCount  = 0;
};
static_assert( sizeof( SampleLoop ) == 24, "sizeof(SampleLoop) is wrong" );


//----------------------------------------------------------------------------------------------------------------------

struct SampleChunk
{
    int32_t manufacturer      = 0;
    int32_t product           = 0;
    int32_t samplePeriod      = 0;
    int32_t midiUnityNote     = 0;
    int32_t midiPitchFraction = 0;
    int32_t smpteFormat       = 0;
    int32_t smpteOffset       = 0;
    int32_t sampleLoops       = 0;
    int32_t samplerData       = 0;
};
static_assert( sizeof( SampleChunk ) == 36, "sizeof(SampleChunk) is wrong" );


//----------------------------------------------------------------------------------------------------------------------

struct InstrumentChunk
{
    uint8_t rootNote     = 0;
    int8_t  fineTune     = 0;
    int8_t  gain         = 0;
    uint8_t lowNote      = 0;
    uint8_t highNote     = 0;
    uint8_t lowVelocity  = 0;
    uint8_t highVelocity = 0;
};
static_assert( sizeof( InstrumentChunk ) == 7, "sizeof(InstrumentChunk) is wrong" );

//----------------------------------------------------------------------------------------------------------------------

struct FormatExtensible
{
    uint16_t validBitsPerSample = 0;
    uint32_t channelMask        = 0;
    uint8_t  subFormat[16]      = {}; // a GUID in fact
};
static_assert( sizeof( FormatExtensible ) == 22, "sizeof(FormatExtensible) is wrong" );

//----------------------------------------------------------------------------------------------------------------------

#pragma pack( pop )

} // namespace wav

//----------------------------------------------------------------------------------------------------------------------
