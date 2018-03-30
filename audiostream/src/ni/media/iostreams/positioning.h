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

#include <cassert>

#include <boost/algorithm/clamp.hpp>
#include <boost/iostreams/positioning.hpp>

inline std::streampos absolute_position( std::streampos                  pos,
                                         std::streampos                  beg,
                                         std::streampos                  end,
                                         boost::iostreams::stream_offset off,
                                         BOOST_IOS::seekdir              way )
{

    switch ( way )
    {
        case BOOST_IOS::beg:
            pos = beg + boost::iostreams::offset_to_position( off );
            break;
        case BOOST_IOS::end:
            pos = end + boost::iostreams::offset_to_position( off );
            break;
        case BOOST_IOS::cur:
            pos += boost::iostreams::offset_to_position( off );
            break;
        default:
            assert( "This should never be reached. It is here to silence a warning from older versions of libstdc++ "
                    "about '_S_ios_seekdir_en' not being handled." );
            return -1;
    }

    return boost::algorithm::clamp( pos, beg, end );
}
