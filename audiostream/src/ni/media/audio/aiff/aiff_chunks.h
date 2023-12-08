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

#include <ni/media/audio/fourcc.h>

#include <boost/endian/arithmetic.hpp>

#include <cstdint>


//----------------------------------------------------------------------------------------------------------------------
// AIFF Chunks
// http://www.paulbourke.net/dataformats/audio/
// http://muratnkonar.com/aiff/index.html
//----------------------------------------------------------------------------------------------------------------------

namespace aiff
{

namespace tags
{

constexpr auto aifc = big_endian_fourcc( "AIFC" );
constexpr auto aiff = big_endian_fourcc( "AIFF" );
constexpr auto form = big_endian_fourcc( "FORM" );
constexpr auto comm = big_endian_fourcc( "COMM" );
constexpr auto mark = big_endian_fourcc( "MARK" );
constexpr auto inst = big_endian_fourcc( "INST" );
constexpr auto ssnd = big_endian_fourcc( "SSND" );
constexpr auto none = big_endian_fourcc( "NONE" );
constexpr auto raw  = big_endian_fourcc( "raw " );
constexpr auto twos = big_endian_fourcc( "twos" );
constexpr auto sowt = big_endian_fourcc( "sowt" );
constexpr auto fl32 = big_endian_fourcc( "fl32" );
constexpr auto FL32 = big_endian_fourcc( "FL32" );
constexpr auto fl64 = big_endian_fourcc( "fl64" );
constexpr auto FL64 = big_endian_fourcc( "FL64" );
constexpr auto in24 = big_endian_fourcc( "in24" );
constexpr auto in32 = big_endian_fourcc( "in32" );

} // namespace tags

struct Tag
{
    boost::endian::big_uint32_t id      = 0;
    boost::endian::big_uint32_t length  = 0;
    boost::endian::big_uint32_t subType = 0;
};


//----------------------------------------------------------------------------------------------------------------------

// The following structs are used in disk operations so we need to prevent the compiler from expanding them

#pragma pack( push, 1 )

struct CommonChunk
{
    boost::endian::big_uint16_t numChannels     = 0;
    boost::endian::big_uint32_t numSampleFrames = 0;
    boost::endian::big_uint16_t sampleSize      = 0;
    uint8_t                     extSampleRate[10]{};
};

static_assert( sizeof( CommonChunk ) == 18, "sizeof(CommonChunk) is wrong" );


//----------------------------------------------------------------------------------------------------------------------

struct Marker
{
    boost::endian::big_int16_t  id       = 0;
    boost::endian::big_uint32_t position = 0;
};

static_assert( sizeof( Marker ) == 6, "sizeof(Marker) is wrong" );


//----------------------------------------------------------------------------------------------------------------------

struct Loop
{
    boost::endian::big_int16_t playMode          = 0;
    boost::endian::big_int16_t markerIDBeginLoop = 0;
    boost::endian::big_int16_t markerIDEndLoop   = 0;
};

static_assert( sizeof( Loop ) == 6, "sizeof(Loop) is wrong" );


//----------------------------------------------------------------------------------------------------------------------

struct InstrumentChunk
{
    boost::endian::big_uint8_t rootNote     = 0;
    boost::endian::big_uint8_t detune       = 0;
    boost::endian::big_uint8_t lowNote      = 0;
    boost::endian::big_uint8_t highNote     = 0;
    boost::endian::big_uint8_t lowVelocity  = 0;
    boost::endian::big_uint8_t highVelocity = 0;
    boost::endian::big_int16_t gain         = 0;
    Loop                       sustainLoop;
    Loop                       releaseLoop;
};

static_assert( sizeof( InstrumentChunk ) == 20, "sizeof(InstrumentChunk) is wrong" );


//----------------------------------------------------------------------------------------------------------------------

struct SoundDataChunk
{
    boost::endian::big_uint32_t offset    = 0;
    boost::endian::big_uint32_t blockSize = 0;
};

static_assert( sizeof( SoundDataChunk ) == 8, "sizeof(SoundDataChunk) is wrong" );


//----------------------------------------------------------------------------------------------------------------------

#pragma pack( pop )

} // namespace aiff

//----------------------------------------------------------------------------------------------------------------------
