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

#include <ni/media/audio/source/ogg_file_source.h>

#include <ni/media/iostreams/positioning.h>

#include <boost/algorithm/clamp.hpp>
#include <boost/optional.hpp>

#define OV_EXCLUDE_STATIC_CALLBACKS 1
#include <vorbis/vorbisfile.h>
#undef OV_EXCLUDE_STATIC_CALLBACKS

class ogg_file_source::Impl
{
public:
    Impl( const std::string& path )
    {
        open( path );
    }

    ~Impl()
    {
        clearVorbisFile();
    }


    void clearVorbisFile();
    void readInfo();
    void open( const std::string& path );

    audio::ifstream_info info();
    std::streamsize      read( char_type* dst, std::streamsize size );
    std::streampos       seek( offset_type offset, BOOST_IOS::seekdir way );

private:
    std::streampos                  m_pos = 0;
    boost::optional<OggVorbis_File> m_vorbisFile;
    audio::ifstream_info            m_info;
};

//----------------------------------------------------------------------------------------------------------------------

ogg_file_source::ogg_file_source() = default;

ogg_file_source::~ogg_file_source() = default;

ogg_file_source::ogg_file_source( ogg_file_source&& ) = default;

ogg_file_source::ogg_file_source( const std::string& path )
: m_impl( new Impl( path ) )
{
}

//----------------------------------------------------------------------------------------------------------------------

void ogg_file_source::open( const std::string& path )
{
    m_impl->open( path );
}

//----------------------------------------------------------------------------------------------------------------------

auto ogg_file_source::read( char_type* dst, std::streamsize size ) -> std::streamsize
{
    return m_impl->read( dst, size );
}

//----------------------------------------------------------------------------------------------------------------------

auto ogg_file_source::seek( offset_type offset, BOOST_IOS::seekdir way ) -> std::streampos
{
    return m_impl->seek( offset, way );
}

//----------------------------------------------------------------------------------------------------------------------

auto ogg_file_source::info() const -> info_type
{
    return m_impl ? m_impl->info() : info_type{};
}

//----------------------------------------------------------------------------------------------------------------------

audio::ifstream_info ogg_file_source::Impl::info()
{
    return m_info;
}


//----------------------------------------------------------------------------------------------------------------------

void ogg_file_source::Impl::open( const std::string& path )
{
    m_pos = 0;

    clearVorbisFile();

    OggVorbis_File vorbisFile;
    if (::ov_fopen( path.c_str(), &vorbisFile ) == 0 )
    {
        m_vorbisFile = vorbisFile;
        readInfo();
    }
    else
    {
        throw std::runtime_error( "Could not open file." );
    }
}

//----------------------------------------------------------------------------------------------------------------------

std::streampos ogg_file_source::Impl::seek( offset_type off, BOOST_IOS::seekdir way )
{
    assert( 0 == off % m_info.bytes_per_frame() );

    if ( !m_vorbisFile )
    {
        throw std::runtime_error( "No vorbis file loaded." );
    }

    const auto beg = std::streampos( 0 );
    const auto end = std::streampos( info().num_bytes() );
    const auto pos = absolute_position( m_pos, beg, end, off, way );

    if (::ov_pcm_seek( m_vorbisFile.get_ptr(), pos / info().bytes_per_frame() ) == 0 )
    {
        m_pos = pos;
    }

    return m_pos;
}

//----------------------------------------------------------------------------------------------------------------------

std::streamsize ogg_file_source::Impl::read( char_type* dst, std::streamsize size )
{
    assert( 0 == size % m_info.bytes_per_frame() );

    if ( !m_vorbisFile )
    {
        return std::streamsize( 0 );
    }

    int  bitstream;
    long delivered = 0;
    int  requested = int( size );
    bool eof       = false;

    while ( !eof )
    {
        long ret = ::ov_read( m_vorbisFile.get_ptr(), dst, requested >= 4096 ? 4096 : requested, 0, 2, 1, &bitstream );
        if ( ret > 0 )
        {
            dst += ret;
            requested -= ret;
            delivered += ret;
        }
        else
        {
            eof = true;
        }
    }

    m_pos += delivered;

    return std::streamsize( delivered );
}

//----------------------------------------------------------------------------------------------------------------------

void ogg_file_source::Impl::clearVorbisFile()
{
    if ( m_vorbisFile )
    {
        ::ov_clear( m_vorbisFile.get_ptr() );
        m_vorbisFile.reset();
    }
}

//----------------------------------------------------------------------------------------------------------------------

void ogg_file_source::Impl::readInfo()
{
    if ( !m_vorbisFile )
    {
        throw std::runtime_error( "No vorbis file loaded." );
    }

    vorbis_info* vinfo = ::ov_info( m_vorbisFile.get_ptr(), -1 );

    if ( !vinfo )
    {
        throw std::runtime_error( "Could not retreive vorbis info." );
    }

    audio::ifstream_info info;
    info.container( audio::ifstream_info::container_type::ogg );
    info.codec( audio::ifstream_info::codec_type::vorbis );
    info.lossless( false );
    info.sample_rate( vinfo->rate );
    info.num_frames( size_t(::ov_pcm_total( m_vorbisFile.get_ptr(), -1 ) ) );
    info.num_channels( vinfo->channels );
    info.format( pcm::make_format<pcm::signed_integer, pcm::_16bit, pcm::little_endian>() );

    m_info = info;
}

//----------------------------------------------------------------------------------------------------------------------
