//
// Copyright (c) 2020 Native Instruments GmbH, Berlin
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

#include <ni/media/audio/ofstream_info.h>

#include <boost/iostreams/device/null.hpp>

#include <memory>

class flac_file_sink
{
public:
    using char_type = char;
    struct category : boost::iostreams::output, boost::iostreams::device_tag, boost::iostreams::closable_tag
    {
    };

    using info_type = audio::ofstream_info;

    flac_file_sink();
    ~flac_file_sink();

    flac_file_sink( flac_file_sink&& );
    explicit flac_file_sink( info_type info, const std::string& path );

    void open( const std::string& path );
    void close();

    auto write( const char_type* s, std::streamsize n ) -> std::streamsize;

    auto info() const -> info_type;

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
    info_type             m_info;
};
