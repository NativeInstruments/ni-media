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

#if NIMEDIA_ENABLE_AIFF_DECODING
#include <ni/media/audio/source/aiff_file_source.h>
#include <ni/media/audio/source/aiff_vector_source.h>
#endif

#if NIMEDIA_ENABLE_FLAC_DECODING
#include <ni/media/audio/source/flac_file_source.h>
#endif

#if NIMEDIA_ENABLE_MP3_DECODING
#include <ni/media/audio/source/mp3_file_source.h>
#endif

#if NIMEDIA_ENABLE_MP4_DECODING
#include <ni/media/audio/source/mp4_file_source.h>
#endif

#if NIMEDIA_ENABLE_OGG_DECODING
#include <ni/media/audio/source/ogg_file_source.h>
#endif

#if NIMEDIA_ENABLE_WAV_DECODING
#include <ni/media/audio/source/wav_file_source.h>
#include <ni/media/audio/source/wav_vector_source.h>
#endif

#if NIMEDIA_ENABLE_WMA_DECODING
#include <ni/media/audio/source/wma_file_source.h>
#endif

#if NIMEDIA_ENABLE_ITUNES_DECODING
#include <ni/media/audio/source/avassetreader_source.h>
#endif

#include <ni/media/audio/source/container_source.h>
