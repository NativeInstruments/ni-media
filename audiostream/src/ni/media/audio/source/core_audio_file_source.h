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

#include <AudioToolbox/ExtendedAudioFile.h>

#include <ni/media/audio/source/mp4_file_source.h>


class core_audio_file_source
{
    using offset_type = boost::iostreams::stream_offset;

public:
    core_audio_file_source( const std::string&                   path,
                            audio::ifstream_info::container_type container,
                            size_t                               stream = 0 );
    ~core_audio_file_source();

    audio::ifstream_info info() const;

    std::streampos  seek( offset_type, BOOST_IOS::seekdir );
    std::streamsize read( char*, std::streamsize );

private:
    ExtAudioFileRef      m_media;
    audio::ifstream_info m_info;
    offset_type          m_framePos = 0;
};
