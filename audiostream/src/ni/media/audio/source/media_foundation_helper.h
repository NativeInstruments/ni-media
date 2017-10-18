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

#ifdef WINVER
#undef WINVER
#endif

#define WINVER _WIN32_WINNT_WIN7

#include <mfapi.h>
#include <mferror.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <windows.h>

#include <Propvarutil.h>
#include <Wmcodecdsp.h>

#include <initguid.h>

// Alac is only defined in widows10 sdk and higher
#ifndef MFAudioFormat_ALAC
DEFINE_MEDIATYPE_GUID( MFAudioFormat_ALAC, 0x6C61 );
#endif

// Flac is only defined in widows10 sdk and higher
#ifndef MFAudioFormat_FLAC
DEFINE_MEDIATYPE_GUID( MFAudioFormat_FLAC, 0xF1AC );
#endif

// windows header defines min / max macros.
#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif
