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

#include <ni/media/audio/ofstream_info.h>
#include <ni/media/audio/ostream.h>

#include <string>

namespace audio
{

class ofstream : public ostream
{
public:
    using info_type = ofstream_info;

    ofstream();

    ofstream( const std::string& file, const ofstream::info_type& info );
    ofstream( const std::string&         file,
              const ofstream::info_type& info,
              info_type::container_type  container,
              size_t                     stream_index = 0 );

    ofstream( ofstream&& );
    ofstream& operator=( ofstream&& );

    const info_type& info() const override;

protected:
    ofstream( std::unique_ptr<streambuf>, std::unique_ptr<info_type> );
};

} // namespace audio
