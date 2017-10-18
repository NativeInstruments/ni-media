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

#include <ni/media/audio/fourcc.h>
#include <ni/media/audio/wav/wav_chunks.h>
#include <ni/media/audio/wav/wav_format.h>
#include <ni/media/audio/wav/wav_ofstream_info.h>
#include <ni/media/iostreams/device/subview.h>
#include <ni/media/iostreams/write_obj.h>

#include <boost/range/algorithm/equal.hpp>

#include <cassert>
#include <cmath>


namespace detail
{

template <class Sink>
auto writeWavHeader( Sink& sink )
{
    // write the riff tag
    auto curChunk = little_endian_fourcc( "RIFF" );
    write_obj( sink, curChunk );

    // grab the offset of the chunkSize header
    sink.chunk_size_offset( sink.tell() );


    // this gets overwritten in the close() call
    // make space for filesize and initialize with 0 for now
    write_obj( sink, uint32_t( 0 ) );

    // WAVE
    curChunk = little_endian_fourcc( "WAVE" );
    write_obj( sink, curChunk );

    // fmt\0 aka subChunk1ID
    curChunk = little_endian_fourcc( "fmt " );
    write_obj( sink, curChunk );

    // subChunk1Size, 16 if PCM
    uint32_t subChunk1Size = 16;
    write_obj( sink, subChunk1Size );

    // audioFormat, PCM = 1
    uint16_t audioFormat = 1;
    write_obj( sink, audioFormat );

    // numChannels
    uint16_t numChannels = static_cast<uint16_t>( sink.info().num_channels() );
    write_obj( sink, numChannels );

    // sampleRate
    uint32_t sampleRate = static_cast<uint32_t>( sink.info().sample_rate() );
    write_obj( sink, sampleRate );

    // byteRate = SampleRate * NumChannels * bytesPerSample
    uint32_t byteRate = static_cast<uint32_t>( sampleRate * numChannels * ( sink.info().bytes_per_sample() ) );
    write_obj( sink, byteRate );

    // blockalign = numChannels * bytesPerSample
    uint16_t blockAlign = static_cast<uint16_t>( numChannels * ( sink.info().bytes_per_sample() ) );
    write_obj( sink, blockAlign );

    // bits per sample
    uint16_t bits_per_sample = static_cast<uint16_t>( sink.info().bits_per_sample() );
    write_obj( sink, bits_per_sample );

    // DATA, aka subChunk2Tag
    curChunk = little_endian_fourcc( "data" );
    write_obj( sink, curChunk );

    // get the current offset to write later
    sink.data_offset( sink.tell() );

    // SubChunk2Size
    write_obj( sink, uint32_t( 0 ) );

    // full header size
    sink.header_offset( sink.tell() );
}

template <class Sink>
auto close( Sink& sink )
{
    // grab the full file size
    uint32_t fileSize = static_cast<uint32_t>( sink.tell() );

    // we need to write the data size for the WAV file before closing
    // seek to the chunkSize offset
    sink.seek( sink.chunk_size_offset(), BOOST_IOS::beg );

    // chunkSize = filesize - 8 (4 bytes for RIFF and 4 bytes for chunkSize)
    uint32_t chunkSize = fileSize - 8;
    write_obj( sink, chunkSize );

    // seek to the data offset
    sink.seek( sink.data_offset(), BOOST_IOS::beg );

    // dataSize = filesize - header size
    uint32_t dataSize = fileSize - static_cast<uint32_t>( sink.header_offset() );
    write_obj( sink, dataSize );
}

} // namespace detail

template <class Sink>
class wav_sink : public boostext::iostreams::subview_sink<Sink>
{
    using base_type   = boostext::iostreams::subview_sink<Sink>;
    using offset_type = typename base_type::offset_type;

public:
    using info_type = audio::wav_ofstream_info;

    template <class... Args>
    explicit wav_sink( info_type info, Args&&... args )
    : base_type( std::forward<Args>( args )... )
    , m_info( info )
    {
        detail::writeWavHeader( *this );
        auto pos = this->tell();
        this->set_view( pos );
    }

    void close()
    {
        this->set_view( 0 );
        detail::close( *this );
        base_type::close();
        m_info = info_type();
    }

    auto info() const -> info_type
    {
        return m_info;
    }

    auto header_offset() const -> offset_type
    {
        return m_header_offset;
    }

    void header_offset( const offset_type offs )
    {
        m_header_offset = offs;
    }

    auto data_offset() const -> offset_type
    {
        return m_data_offset;
    }

    void data_offset( const offset_type offs )
    {
        m_data_offset = offs;
    }

    auto chunk_size_offset() const -> offset_type
    {
        return m_chunk_size_offset;
    }

    void chunk_size_offset( const offset_type offs )
    {
        m_chunk_size_offset = offs;
    }

private:
    info_type m_info;

    offset_type m_header_offset     = 0;
    offset_type m_data_offset       = 0;
    offset_type m_chunk_size_offset = 0;
};
