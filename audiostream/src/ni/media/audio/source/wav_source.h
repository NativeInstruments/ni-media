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
#include <ni/media/audio/wav/wav_ifstream_info.h>
#include <ni/media/iostreams/device/subview.h>
#include <ni/media/iostreams/fetch.h>

#include <boost/range/algorithm/equal.hpp>

#include <cassert>
#include <cmath>


namespace detail
{

template <class Source>
auto readWavHeader( Source& src )
{
    using namespace wav;
    using SampleLoops = audio::wav_specific_info::SampleLoops;

    audio::wav_ifstream_info info;
    src.seek( 0, std::ios::beg );

    // read riff chunk
    RiffTag riffTag;
    if ( !fetch( src, riffTag.id, riffTag.length ) || riffTag.id != little_endian_fourcc( "RIFF" ) )
        throw std::runtime_error( "Could not read \'RIFF\' tag." );

    // read wave chunk
    if ( !fetch( src, riffTag.id ) || riffTag.id != little_endian_fourcc( "WAVE" ) )
        throw std::runtime_error( "Could not read \'WAVE\' tag." );

    while ( fetch( src, riffTag.id, riffTag.length ) && riffTag.length > 0 )
    {
        const auto currentOffset = static_cast<uint32_t>( src.seek( 0, std::ios_base::cur ) );

        // seek through wav chunks
        if ( riffTag.id == little_endian_fourcc( "fmt " ) )
        {
            FmtChunk fmtChunk;
            if ( !fetch( src, fmtChunk ) )
                throw std::runtime_error( "Could not read \'fmtChunk\'." );

            if ( fmtChunk.blockAlign == 0 || fmtChunk.blockAlign * 8 != fmtChunk.bitsPerSample * fmtChunk.numChannels )
                throw std::runtime_error( "Invalid block align" );

            // determine format
            audio::ifstream_info::format_type format;
            if ( fmtChunk.formatTag == wavFormatTagExtensible )
            {
                // read extended chunk
                FormatExtensible formatExtensible;
                if ( !fetch( src, formatExtensible ) )
                    throw std::runtime_error( "Could not read \'formatExtensible\'." );

                if ( boost::equal( formatExtensible.subFormat, wavFormatExtSubFormatPCM )
                     || boost::equal( formatExtensible.subFormat, wavFormatExtSubFormatPCM_2 ) )
                {
                    fmtChunk.formatTag = wavFormatTagPcm;
                }
                else if ( boost::equal( formatExtensible.subFormat, wavFormatExtSubFormatFloat )
                          || boost::equal( formatExtensible.subFormat, wavFormatExtSubFormatFloat_2 ) )
                {
                    fmtChunk.formatTag = wavFormatTagIeeeFloat;
                }
            }

            if ( fmtChunk.formatTag == wavFormatTagPcm )
            {
                if ( 8 == fmtChunk.bitsPerSample )
                    format = pcm::format( pcm::unsigned_integer, fmtChunk.bitsPerSample );
                else
                    format = pcm::format( pcm::signed_integer, fmtChunk.bitsPerSample, pcm::little_endian );
            }
            else if ( fmtChunk.formatTag == wavFormatTagIeeeFloat )
            {
                format = pcm::format( pcm::floating_point, fmtChunk.bitsPerSample, pcm::little_endian );
            }
            else
                throw std::runtime_error( "Unknown format." );

            info.format( format );
            info.num_channels( fmtChunk.numChannels );
            info.sample_rate( fmtChunk.sampleRate );
        }
        else if ( riffTag.id == little_endian_fourcc( "data" ) )
        {
            if ( info.bytes_per_frame() == 0 )
                throw std::runtime_error( "Invalid \'smplChunk\'." );

            info.num_frames( riffTag.length / info.bytes_per_frame() );
            return info;
        }
        else if ( riffTag.id == little_endian_fourcc( "smpl" ) )
        {
            if ( riffTag.length >= sizeof( SampleChunk ) )
            {
                SampleChunk sampleChunk;
                if ( !fetch( src, sampleChunk ) )
                    throw std::runtime_error( "Could not read \'sampleChunk\'." );

                info.sample_chunk( *reinterpret_cast<const audio::wav_specific_info::SampleChunk*>( &sampleChunk ) );

                if ( sampleChunk.sampleLoops > 0 )
                {
                    SampleLoops sampleLoops;

                    for ( int i = 0; i < sampleChunk.sampleLoops; ++i )
                    {
                        SampleLoop sample_loop_raw;
                        if ( !fetch( src, sample_loop_raw ) )
                            throw std::runtime_error( "Could not read one of the loops" );

                        audio::wav_specific_info::SampleLoop sample_loop{sample_loop_raw.type,
                                                                         sample_loop_raw.start,
                                                                         sample_loop_raw.end,
                                                                         sample_loop_raw.fraction,
                                                                         sample_loop_raw.playCount};

                        const auto id = sample_loop_raw.identifier;
                        sampleLoops.insert( SampleLoops::value_type{id, sample_loop} );
                    }

                    info.sample_loops( sampleLoops );
                }
            }
        }
        else if ( riffTag.id == little_endian_fourcc( "inst" ) )
        {
            if ( riffTag.length >= sizeof( InstrumentChunk ) )
            {
                InstrumentChunk instrumentChunk;
                if ( !fetch( src, instrumentChunk ) )
                    throw std::runtime_error( "Could not read \'instrumentChunk\'." );

                info.instrument_chunk(
                    *reinterpret_cast<const audio::wav_specific_info::InstrumentChunk*>( &instrumentChunk ) );
            }
        }

        src.seek( ( currentOffset + riffTag.length + 1 ) & 0xfffffe, std::ios_base::beg );
    }

    throw std::runtime_error( "Could not read \'data\' tag." );
}

} // namespace detail

template <class Source>
class wav_source : public boostext::iostreams::subview_source<Source>
{
    using base_type = boostext::iostreams::subview_source<Source>;

public:
    using info_type = audio::wav_ifstream_info;
    using char_type = typename base_type::char_type;

    template <class... Args>
    explicit wav_source( Args&&... args )
    : base_type( std::forward<Args>( args )... )
    {
        m_info   = detail::readWavHeader( *this );
        auto pos = this->seek( 0, std::ios_base::cur );
        this->set_view( pos, pos + std::streamoff( m_info.num_bytes() ) );
    }

    std::streamsize read( char_type* s, std::streamsize n )
    {
        assert( 0 == n % m_info.bytes_per_frame() );
        auto r = base_type::read( s, n );
        return r < 0 ? r : r - r % m_info.bytes_per_frame();
    }

    auto info() const
    {
        return m_info;
    }

private:
    info_type m_info;
};
