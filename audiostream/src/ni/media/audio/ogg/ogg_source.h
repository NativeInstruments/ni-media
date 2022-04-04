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

#include <ni/media/iostreams/positioning.h>

#include <ni/media/audio/ifstream_info.h>

#include <boost/algorithm/clamp.hpp>
#include <boost/iostreams/categories.hpp>
#include <boost/optional.hpp>

#define OV_EXCLUDE_STATIC_CALLBACKS 1
#include <vorbis/vorbisfile.h>
#undef OV_EXCLUDE_STATIC_CALLBACKS

#include <array>
#include <stdexcept>
#include <string>

template <class Source>
class ogg_source
{
public:
    using char_type = char;
    struct category : boost::iostreams::input_seekable, boost::iostreams::device_tag
    {
    };

    using offset_type = boost::iostreams::stream_offset;
    using info_type   = audio::ifstream_info;

    ogg_source( Source&& );
    ~ogg_source();

    void clearVorbisFile();
    void readInfo();
    void open( const std::string& path );

    audio::ifstream_info info();
    std::streamsize      read( char_type* dst, std::streamsize size );
    std::streampos       seek( offset_type offset, BOOST_IOS::seekdir way );

private:
    Source                          m_source;
    std::streampos                  m_pos = 0;
    boost::optional<OggVorbis_File> m_vorbisFile;
    audio::ifstream_info            m_info;
};

template <class Source>
size_t readCallback( void* buffer, size_t elementSize, size_t elementCount, void* clientData )
{
    auto source  = static_cast<Source*>( clientData );
    auto numRead = source->read( static_cast<char*>( buffer ), elementSize * elementCount );
    return numRead > 0 ? static_cast<size_t>( numRead ) : 0;
}

template <class Source>
int seekCallback( void* clientData, ogg_int64_t offset, int origin )
{
    constexpr std::array<std::ios_base::seekdir, 3> seekDirections{
        {std::ios_base::beg, std::ios_base::cur, std::ios_base::end}};

    auto source = static_cast<Source*>( clientData );
    auto pos    = source->seek( boost::iostreams::stream_offset( offset ), seekDirections.at( origin ) );
    return pos < 0 ? -1 : 0;
}

template <class Source>
long tellCallback( void* clientData )
{
    auto       source   = static_cast<Source*>( clientData );
    const auto position = source->seek( boost::iostreams::stream_offset( 0 ), BOOST_IOS::cur );
    assert( position >= 0 );
    return static_cast<long>( position );
}

//----------------------------------------------------------------------------------------------------------------------
template <class Source>
ogg_source<Source>::ogg_source( Source&& source )
: m_source( std::move( source ) )
{
    clearVorbisFile();

    OggVorbis_File vorbisFile;

    const ov_callbacks callbacks{readCallback<Source>, seekCallback<Source>, nullptr, tellCallback<Source>};
    int                result = ov_open_callbacks( this, &vorbisFile, nullptr, 0, callbacks );

    if ( result == 0 )
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

template <class Source>
ogg_source<Source>::ogg_source::~ogg_source()
{
    clearVorbisFile();
}

//----------------------------------------------------------------------------------------------------------------------

template <class Source>
audio::ifstream_info ogg_source<Source>::info()
{
    return m_info;
}


//----------------------------------------------------------------------------------------------------------------------

template <class Source>
std::streampos ogg_source<Source>::seek( offset_type off, BOOST_IOS::seekdir way )
{
    assert( 0 == off % m_info.bytes_per_frame() );

    if ( !m_vorbisFile )
    {
        throw std::runtime_error( "No vorbis file loaded." );
    }

    const auto beg = std::streampos( 0 );
    const auto end = std::streampos( info().num_bytes() );
    const auto pos = absolute_position( m_pos, beg, end, off, way );

    if ( ::ov_pcm_seek( m_vorbisFile.get_ptr(), pos / info().bytes_per_frame() ) == 0 )
    {
        m_pos = pos;
    }

    return m_pos;
}

//----------------------------------------------------------------------------------------------------------------------

template <class Source>
std::streamsize ogg_source<Source>::read( char_type* dst, std::streamsize size )
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

template <class Source>
void ogg_source<Source>::clearVorbisFile()
{
    if ( m_vorbisFile )
    {
        ::ov_clear( m_vorbisFile.get_ptr() );
        m_vorbisFile.reset();
    }
}

//----------------------------------------------------------------------------------------------------------------------

template <class Source>
void ogg_source<Source>::readInfo()
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

    const ogg_int64_t num_frames = ::ov_pcm_total( m_vorbisFile.get_ptr(), -1 );

    if ( num_frames < 1 )
        throw std::runtime_error( "Invalid / Empty File" );

    // monaural, polyphonic, stereo, quadraphonic, 5.1, ambisonic, or up to 255 discrete channels.
    if ( vinfo->channels < 1 || vinfo->channels > 255 )
        throw std::runtime_error( "Invalid number of channels: " + std::to_string( vinfo->channels ) );

    // Vorbis is also intended for lower and higher sample rates (from 8kHz telephony to 192kHz digital masters)
    if ( vinfo->rate < 8000u || vinfo->rate > 192000u )
        throw std::runtime_error( "Invalid sample rate: " + std::to_string( vinfo->rate ) );


    audio::ifstream_info info;
    info.container( audio::ifstream_info::container_type::ogg );
    info.codec( audio::ifstream_info::codec_type::vorbis );
    info.lossless( false );
    info.sample_rate( size_t( vinfo->rate ) );
    info.num_frames( size_t( num_frames ) );
    info.num_channels( size_t( vinfo->channels ) );
    info.format( pcm::make_format<pcm::signed_integer, pcm::_16bit, pcm::little_endian>() );
    info.bit_rate( vinfo->bitrate_nominal );

    m_info = std::move( info );
}

//----------------------------------------------------------------------------------------------------------------------
