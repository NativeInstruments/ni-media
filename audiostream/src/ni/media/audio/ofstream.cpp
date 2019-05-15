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

#include <ni/media/audio/iotools.h>
#include <ni/media/audio/ofstream.h>

#include <ni/media/audio/sink.h>

#include <boost/predef.h>

namespace audio
{

//----------------------------------------------------------------------------------------------------------------------

ofstream::ofstream()
: ostream( nullptr, std::make_unique<info_type>() )
{
}

//----------------------------------------------------------------------------------------------------------------------


ofstream::ofstream( ofstream&& other )
: ostream( std::move( other ) )
{
}

//----------------------------------------------------------------------------------------------------------------------

ofstream& ofstream::operator=( ofstream&& other )
{
    ostream::operator=( std::move( other ) );
    return *this;
}

//----------------------------------------------------------------------------------------------------------------------

ofstream::ofstream( std::unique_ptr<streambuf> sb, std::unique_ptr<ofstream::info_type> info )
: ostream( std::move( sb ), std::move( info ) )
{
}

//----------------------------------------------------------------------------------------------------------------------

const ofstream::info_type& ofstream::info() const
{
    return static_cast<const info_type&>( ostream::info() );
}

} // namespace pcm
