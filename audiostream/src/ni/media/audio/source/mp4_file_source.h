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

#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/positioning.hpp>
#include <boost/predef.h>

#include <memory>

class mp4_file_source
{
public:
    using char_type = char;
    struct category : boost::iostreams::input_seekable, boost::iostreams::device_tag, boost::iostreams::closable_tag
    {
    };

    using info_type = audio::ifstream_info;

    mp4_file_source();
    ~mp4_file_source();

    mp4_file_source( mp4_file_source&& );
    explicit mp4_file_source( const std::string& path, size_t stream = 0 );

    void open( const std::string& path, size_t stream = 0 );
    void close();

    bool is_open() const;

    auto seek( boost::iostreams::stream_offset, BOOST_IOS::seekdir = BOOST_IOS::beg ) -> std::streampos;
    auto read( char_type*, std::streamsize ) -> std::streamsize;

    auto info() const -> info_type;

private:
#if BOOST_OS_MACOS || BOOST_OS_IOS
    std::unique_ptr<class core_audio_file_source> m_impl;
#elif BOOST_OS_WINDOWS
    std::unique_ptr<class media_foundation_file_source> m_impl;
#endif
};
