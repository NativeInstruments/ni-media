//
// Copyright (c) 2017-2019 Native Instruments GmbH, Berlin
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
#include <ni/media/audio/istream.h>
#include <ni/media/audio/istream_source.h>
#include <ni/media/audio/custom_backend_source.h>

#include <string>

namespace audio
{

class ifstream;

using ifstream_source = istream_source<ifstream>;

class ifstream : public istream
{
public:
    using info_type = ifstream_info;

    ifstream();

    ifstream( const std::string& file );
    ifstream( const std::string& file, info_type::container_type container, size_t stream_index = 0 );
    ifstream( std::unique_ptr<ifstream_source> source );
    ifstream( std::unique_ptr<custom_backend_source> source, info_type::container_type container, size_t stream_index = 0 );

    ifstream( ifstream&& );
    ifstream& operator=( ifstream&& );

    const info_type& info() const override;

protected:
    ifstream( std::unique_ptr<streambuf>, std::unique_ptr<info_type> );
};

//----------------------------------------------------------------------------------------------------------------------

template <class Source, class... Args>
ifstream make_ifstream( Args&&... args )
{
    return ifstream( std::make_unique<Source>( std::forward<Args>( args )... ) );
}

} // namespace audio
