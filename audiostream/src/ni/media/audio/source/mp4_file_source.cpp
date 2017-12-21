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

#include <ni/media/audio/source/mp4_file_source.h>

#if BOOST_OS_MACOS || BOOST_OS_IOS
#include "core_audio_file_source.h"
#elif BOOST_OS_WINDOWS
#include "media_foundation_file_source.h"
#elif BOOST_OS_LINUX
#include "gstreamer_file_source.h"
#endif

//----------------------------------------------------------------------------------------------------------------------

mp4_file_source::mp4_file_source() = default;

//----------------------------------------------------------------------------------------------------------------------

mp4_file_source::~mp4_file_source() = default;

//----------------------------------------------------------------------------------------------------------------------

mp4_file_source::mp4_file_source( mp4_file_source&& source ) = default;

//----------------------------------------------------------------------------------------------------------------------

mp4_file_source::mp4_file_source( const std::string& path, size_t stream )
{
    open( path, stream );
}

//----------------------------------------------------------------------------------------------------------------------

void mp4_file_source::open( const std::string& path, size_t stream )
{
#if BOOST_OS_MACOS || BOOST_OS_IOS
    m_impl.reset( new core_audio_file_source( path, audio::ifstream_info::container_type::mp4, stream ) );
#elif BOOST_OS_WINDOWS
    m_impl.reset( new media_foundation_file_source( path, audio::ifstream_info::container_type::mp4, stream ) );
#elif BOOST_OS_LINUX
    m_impl.reset( new gstreamer_file_source( path, audio::ifstream_info::container_type::mp4 ) );
#endif
}

//----------------------------------------------------------------------------------------------------------------------

void mp4_file_source::close()
{
    m_impl.reset();
}

//----------------------------------------------------------------------------------------------------------------------

bool mp4_file_source::is_open() const
{
    return m_impl != nullptr;
}

//----------------------------------------------------------------------------------------------------------------------

auto mp4_file_source::seek( boost::iostreams::stream_offset offset, BOOST_IOS::seekdir way ) -> std::streampos
{
    return m_impl ? m_impl->seek( offset, way ) : std::streampos( 0 );
}

//----------------------------------------------------------------------------------------------------------------------

auto mp4_file_source::read( char_type* dst, std::streamsize size ) -> std::streamsize
{
    return m_impl ? m_impl->read( dst, size ) : 0;
}

//----------------------------------------------------------------------------------------------------------------------

auto mp4_file_source::info() const -> info_type
{
    return m_impl ? m_impl->info() : info_type{};
}
