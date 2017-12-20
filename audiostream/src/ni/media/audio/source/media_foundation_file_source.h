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

#include <boost/icl/interval_set.hpp>
#include <boost/icl/right_open_interval.hpp>
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/positioning.hpp>

#include <ni/media/audio/ifstream_info.h>


struct IMFSourceReader;

class media_foundation_file_source
{
    struct MfInitializer;
    struct MfBlock;
    class MfBuffer;

public:
    //----------------------------------------------------------------------------------------------------------------------

    using offset_type = boost::iostreams::stream_offset;

    using FrameRange    = boost::icl::right_open_interval<offset_type>;
    using FrameRangeSet = boost::icl::interval_set<FrameRange::domain_type, std::less, FrameRange>;

    template <typename MfType>
    using MfTypePtr = std::unique_ptr<MfType, std::function<void( MfType* )>>;

    //----------------------------------------------------------------------------------------------------------------------

    explicit media_foundation_file_source( const std::string&                   path,
                                           audio::ifstream_info::container_type container,
                                           size_t                               stream = 0 );
    ~media_foundation_file_source();

    audio::ifstream_info info() const
    {
        return m_info;
    }

    std::streampos  seek( offset_type, BOOST_IOS::seekdir );
    std::streamsize read( char*, std::streamsize );

private:
    static const offset_type s_defaultBlockSize = 1024;

    std::unique_ptr<MfInitializer> m_initializer;
    std::unique_ptr<MfBuffer>      m_buffer;
    MfTypePtr<IMFSourceReader>     m_reader;

    audio::ifstream_info m_info;
    size_t               m_streamIndex = 0;

    offset_type m_readOffset  = 0;
    offset_type m_nominalPos  = 0;
    offset_type m_adjustedPos = 0;

    bool m_fadein = false;

    bool seekInternal( offset_type );

    auto readFromBuffer( const FrameRange&, char* ) -> FrameRangeSet;
    auto readFromFile( const FrameRange&, char* ) -> FrameRangeSet;

    auto searchAndRetrieveBlock( offset_type ) -> std::unique_ptr<MfBlock>;
    auto searchForwardAndRetrieveBlock( offset_type ) -> std::unique_ptr<MfBlock>;
    auto searchBackwardAndRetrieveBlock( offset_type, offset_type ) -> std::unique_ptr<MfBlock>;

    auto consumeBlock() -> std::unique_ptr<MfBlock>;
    bool updateBuffer( std::unique_ptr<MfBlock> );
    bool discardFadeinBlock();
};
