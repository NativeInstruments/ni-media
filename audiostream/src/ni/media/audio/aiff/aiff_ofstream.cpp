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

#include <ni/media/audio/aiff/aiff_file_sink.h>
#include <ni/media/audio/aiff/aiff_ofstream.h>
#include <ni/media/iostreams/stream_buffer.h>

namespace audio
{

//----------------------------------------------------------------------------------------------------------------------

aiff_ofstream::aiff_ofstream()
: ofstream( nullptr, std::make_unique<info_type>() )
{
}

//----------------------------------------------------------------------------------------------------------------------

aiff_ofstream::aiff_ofstream( aiff_ofstream&& other )
: ofstream( std::move( other ) )
{
}

//----------------------------------------------------------------------------------------------------------------------

aiff_ofstream& aiff_ofstream::operator=( aiff_ofstream&& other )
{
    ofstream::operator=( std::move( other ) );
    return *this;
}

//----------------------------------------------------------------------------------------------------------------------

aiff_ofstream::aiff_ofstream( const std::string& file, const info_type& info )
: ofstream( make_stream_buffer( aiff_file_sink( info, file ) ), make_info( info ) )
{
}

//----------------------------------------------------------------------------------------------------------------------

const aiff_ofstream::info_type& aiff_ofstream::info() const
{
    return static_cast<const info_type&>( ofstream::info() );
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace audio
