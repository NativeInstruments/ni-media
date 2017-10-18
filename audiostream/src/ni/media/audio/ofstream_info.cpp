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

#include <ni/media/audio/ofstream_info.h>

#include <tuple>

namespace audio
{

//----------------------------------------------------------------------------------------------------------------------

void ofstream_info::codec( codec_type value )
{
    m_codec = value;
}

//----------------------------------------------------------------------------------------------------------------------

auto ofstream_info::codec() const -> codec_type
{
    return m_codec;
}

//----------------------------------------------------------------------------------------------------------------------

void ofstream_info::container( container_type value )
{
    m_container = value;
}

//----------------------------------------------------------------------------------------------------------------------

auto ofstream_info::container() const -> container_type
{
    return m_container;
}


//----------------------------------------------------------------------------------------------------------------------

bool operator==( const ofstream_info& lhs, const ofstream_info& rhs )
{
    return static_cast<const ostream_info&>( lhs ) == static_cast<const ostream_info&>( rhs )
           && static_cast<const fstream_info&>( lhs ) == static_cast<const fstream_info&>( rhs );
}

//----------------------------------------------------------------------------------------------------------------------

bool operator!=( const ofstream_info& lhs, const ofstream_info& rhs )
{
    return !( lhs == rhs );
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace audio
