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

#include <ni/media/audio/aiff/aiff_chunks.h>
#include <ni/media/audio/aiff/aiff_ifstream_info.h>
#include <ni/media/audio/ieee80.h>
#include <ni/media/iostreams/device/subview.h>
#include <ni/media/iostreams/fetch.h>

#include <boost/algorithm/cxx11/any_of.hpp>
#include <boost/endian/arithmetic.hpp>
#include <boost/format.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/irange.hpp>

#include <ciso646>
#include <cmath>
#include <cstdint>

//----------------------------------------------------------------------------------------------------------------------

namespace detail
{

template <class Source>
auto read_pascal_string( Source& src ) -> boost::optional<std::string>
{
    uint8_t size;
    if ( !fetch( src, size ) )
        return boost::none;

    char data[256];
    if ( ( size > 0 ) && ( src.read( data, size ) != size ) )
        return boost::none;

    // pascal strings must be even in total byte count
    if ( ( size + 1 ) % 2 )
        src.seek( 1, BOOST_IOS::cur );

    return boost::make_optional( std::string( data, size ) );
}

//----------------------------------------------------------------------------------------------------------------------

template <class Source>
auto readAiffHeader( Source& src )
{
    using namespace boost::endian;
    using MarkerMap = audio::aiff_specific_info::MarkerMap;

    audio::aiff_ifstream_info info;

    src.seek( 0, BOOST_IOS::beg );

    // Read the 'FORM'chunk
    aiff::Tag aiffTag;

    if ( !fetch( src, aiffTag.id, aiffTag.length, aiffTag.subType ) || aiffTag.id != aiff::tags::form )
        throw std::runtime_error( "Could not read \'FORM\' tag." );

    aiffTag.start = src.seek( 0, BOOST_IOS::cur );

    while ( fetch( src, aiffTag.id, aiffTag.length ) )
    {
        // Retrieve the 'COMM' subchunk, which contains the audio format information
        if ( aiffTag.id == aiff::tags::comm )
        {
            static const size_t commChunkSize = sizeof( aiff::CommonChunk );
            aiff::CommonChunk   commChunk;

            if ( aiffTag.length < commChunkSize || !fetch( src, commChunk ) )
            {
                throw std::runtime_error( "Could not read \'COMM\' chunk." );
            }

            pcm::number_type    numberType = pcm::signed_integer;
            size_t              bitWidth = commChunk.sampleSize;
            bool                littleEndian = false;

            if ( aiffTag.subType == aiff::tags::aifc )
            {
                big_uint32_t compression = 0;
                if ( !fetch( src, compression ) )
                    throw std::runtime_error( "Could not retrieve the compression id." );

                if ( aiffTag.length > commChunkSize + 4 )
                {
                    // Read the Pascal-style string containing the compression name
                    if ( !read_pascal_string( src ) )
                        throw std::runtime_error( "Could not retrieve the compressor id." );
                }

                switch ( compression )
                {
                    case aiff::tags::none:
                        break;
                    case aiff::tags::sowt:
                        littleEndian = true;
                        break;
                    case aiff::tags::fl32:
                    case aiff::tags::FL32:
                        bitWidth = 32;
                        numberType = pcm::floating_point;
                        break;
                    case aiff::tags::fl64:
                    case aiff::tags::FL64:
                        bitWidth = 64;
                        numberType = pcm::floating_point;
                        break;
                    default:
                        throw std::runtime_error( "Unsupported compressor." );
                }
            }

            auto format = pcm::format(
                numberType, bitWidth, littleEndian ? pcm::little_endian : pcm::big_endian );

            info.format( format );
            info.num_channels( commChunk.numChannels );
            info.num_frames( commChunk.numSampleFrames );
            info.sample_rate( size_t( ieee_80_to_double( commChunk.extSampleRate ) ) );
        }

        else if ( aiffTag.id == aiff::tags::mark )
        {
            big_uint16_t num_markers = 0;
            if ( fetch( src, num_markers ) && num_markers > 0 )
            {
                MarkerMap marker_map;

                boost::range::for_each( boost::irange( 0, static_cast<int>( num_markers ) ), [&]( const auto& ) {
                    aiff::Marker marker;
                    if ( !fetch( src, marker ) )
                        throw std::runtime_error( "Error reading AIFF marker." );

                    // Read the Pascal-style string containing the marker name
                    if ( !read_pascal_string( src ) )
                        throw std::runtime_error( "Error reading AIFF marker." );

                    marker_map.insert( MarkerMap::value_type{marker.id, marker.position} );
                } );

                info.marker_map( marker_map );
            }
        }

        else if ( aiffTag.id == aiff::tags::inst )
        {
            aiff::InstrumentChunk inst_chunk;

            if ( !fetch( src, inst_chunk ) )
                throw std::runtime_error( "Could not read \'INST\' chunk." );

            audio::aiff_specific_info::InstrumentChunk inst_chunk_native_endian{
                inst_chunk.rootNote,
                inst_chunk.detune,
                inst_chunk.lowNote,
                inst_chunk.highNote,
                inst_chunk.lowVelocity,
                inst_chunk.highVelocity,
                inst_chunk.gain,
                {inst_chunk.sustainLoop.playMode,
                 inst_chunk.sustainLoop.markerIDBeginLoop,
                 inst_chunk.sustainLoop.markerIDEndLoop},
                {inst_chunk.releaseLoop.playMode,
                 inst_chunk.releaseLoop.markerIDBeginLoop,
                 inst_chunk.releaseLoop.markerIDEndLoop}};
            info.instrument_chunk( inst_chunk_native_endian );
        }

        // The SSND chunk is the last in the header
        else if ( aiffTag.id == aiff::tags::ssnd )
        {
            aiff::SoundDataChunk ssndChunck;

            if ( !fetch( src, ssndChunck ) )
                throw std::runtime_error( "Could not read \'SSND\' chunk." );

            src.seek( ssndChunck.offset, BOOST_IOS::cur );

            return info;
        }

        else
        {
            static const size_t padSize   = 2;
            auto                remainder = aiffTag.length % padSize;
            auto                target    = aiffTag.length + ( remainder != 0 ? padSize - remainder : 0 );
            src.seek( target, BOOST_IOS::cur );
        }
    }

    throw std::runtime_error( "Could not read \'data\' tag." );
}


} // namespace detail

template <class Source>
class aiff_source : public boostext::iostreams::subview_source<Source>
{
    using base_type = boostext::iostreams::subview_source<Source>;

public:
    using info_type = audio::aiff_ifstream_info;
    using char_type = typename base_type::char_type;

    template <class... Args>
    explicit aiff_source( Args&&... args )
    : base_type( std::forward<Args>( args )... )
    {
        m_info   = detail::readAiffHeader( *this );
        auto pos = this->seek( 0, std::ios_base::cur );
        this->set_view( pos, pos + std::streamoff( m_info.num_bytes() ) );
    }

    std::streamsize read( char_type* s, std::streamsize n )
    {
        assert( 0 == n % m_info.bytes_per_frame() );
        auto r = base_type::read( s, n );
        return r < 0 ? r : r - r % m_info.bytes_per_frame();
    }

    auto info() const -> info_type
    {
        return m_info;
    }

private:
    info_type m_info;
};
