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
#include <ni/media/iostreams/positioning.h>

#include <boost/algorithm/clamp.hpp>
#include <boost/algorithm/cxx11/any_of.hpp>
#include <boost/format.hpp>
#include <boost/icl/interval_set.hpp>
#include <boost/icl/right_open_interval.hpp>
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/positioning.hpp>
#include <boost/make_unique.hpp>

#include <ni/media/audio/ifstream_info.h>

#include <functional>
#include <stdexcept>
#include <string>

// this header must be included last
#include "media_foundation_helper.h"

//----------------------------------------------------------------------------------------------------------------------

template <class Source>
class media_foundation_source
{
    struct MfInitializer;
    struct MfBlock;
    class MfBuffer;

public:
    //----------------------------------------------------------------------------------------------------------------------

    using info_type = audio::ifstream_info;

    using offset_type = boost::iostreams::stream_offset;

    using FrameRange    = boost::icl::right_open_interval<offset_type>;
    using FrameRangeSet = boost::icl::interval_set<FrameRange::domain_type, std::less, FrameRange>;

    template <typename MfType>
    using MfTypePtr = std::unique_ptr<MfType, std::function<void( MfType* )>>;

    //----------------------------------------------------------------------------------------------------------------------

    media_foundation_source( Source&& source, audio::ifstream_info::container_type container, size_t stream = 0 );
    ~media_foundation_source();

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

    SyncronousByteStream<Source>* m_byteStream;
};


namespace
{
const auto wmaFormats = {MFAudioFormat_WMAudioV8, //
                         MFAudioFormat_WMAudioV9,
                         MFAudioFormat_WMAudio_Lossless};

const auto mp3Formats = {MFAudioFormat_MP3};

const auto mp4Formats = {MFAudioFormat_ALAC, //
                         MFAudioFormat_AAC};

const auto losslessFormats = {MFAudioFormat_PCM, //
                              MFAudioFormat_ALAC,
                              MFAudioFormat_FLAC,
                              MFAudioFormat_WMAudio_Lossless};


//----------------------------------------------------------------------------------------------------------------------
// RAII wrappers for WMF interfaces
//----------------------------------------------------------------------------------------------------------------------

template <class Source, typename MfType>
using MfTypePtr = typename media_foundation_source<Source>::template MfTypePtr<MfType>;

template <class C>
struct get_MF_type_impl : get_MF_type_impl<decltype( &C::operator() )>
{
};

template <class C, typename MFType>
struct get_MF_type_impl<HRESULT ( C::* )( MFType** )> : get_MF_type_impl<HRESULT( MFType** )>
{
};

template <class C, typename MFType>
struct get_MF_type_impl<HRESULT ( C::* )( MFType** ) const> : get_MF_type_impl<HRESULT( MFType** )>
{
};

template <typename MFType>
struct get_MF_type_impl<HRESULT( MFType** )>
{
    using type = MFType;
};

template <typename F>
using get_MF_type = typename get_MF_type_impl<typename std::remove_reference<F>::type>::type;

template <class Source, typename MfAllocator>
auto allocateNoThrow( MfAllocator&& allocator ) -> MfTypePtr<Source, get_MF_type<MfAllocator>>
{
    using MfType = get_MF_type<MfAllocator>;

    MfType* ptr    = nullptr;
    auto    status = allocator( &ptr );

    MfTypePtr<Source, MfType> ret( ptr, []( MfType* ptr ) {
        if ( ptr )
            ptr->Release();
    } );
    return SUCCEEDED( status ) ? std::move( ret ) : nullptr;
}

template <class Source, typename MfAllocator>
auto allocateOrThrow( MfAllocator&& allocator, std::string errorMsg ) -> MfTypePtr<Source, get_MF_type<MfAllocator>>
{
    auto ptr = allocateNoThrow<Source>( std::forward<MfAllocator>( allocator ) );

    if ( !ptr )
        throw std::runtime_error( std::move( errorMsg ) );
    return ptr;
}

//----------------------------------------------------------------------------------------------------------------------

bool setPosition( IMFSourceReader& reader, LONGLONG time100ns )
{
    PROPVARIANT var;
    return SUCCEEDED( InitPropVariantFromInt64( time100ns, &var ) )
           && SUCCEEDED( reader.SetCurrentPosition( GUID_NULL, var ) ) && SUCCEEDED( PropVariantClear( &var ) );
}

template <class Source>
typename media_foundation_source<Source>::offset_type time100nsToFrames( LONGLONG time100ns, size_t sampleRate )
{
    static const LONGLONG secTo100ns = 10000000;

    // We need to be accurate to a single frame when doing this conversion, and the timestamp provided by WMF can be
    // off by a few nanoseconds, so we round it off.

    using return_type = typename media_foundation_source<Source>::offset_type;
    return return_type( std::round( double( time100ns ) * sampleRate / secTo100ns ) );
}

template <class Source>
LONGLONG framesTo100ns( typename media_foundation_source<Source>::offset_type frame, size_t sampleRate )
{
    static const LONGLONG secTo100ns = 10000000;
    return frame * secTo100ns / sampleRate;
}

//----------------------------------------------------------------------------------------------------------------------

template <class Source>
auto retrieveAudioStreamsIndices( IMFSourceReader& reader ) -> std::vector<size_t>
{
    // Retrieve all the indices of the streams in the file that contain audio data and place them in a vector.

    std::vector<size_t> streams;

    DWORD cursor       = 0;
    bool  keepScanning = true;
    do
    {
        GUID attribute;
        auto media =
            allocateNoThrow<Source>( [&]( IMFMediaType** ptr ) { return reader.GetCurrentMediaType( cursor, ptr ); } );
        keepScanning = keepScanning && media && SUCCEEDED( media->GetMajorType( &attribute ) );

        if ( keepScanning && attribute == MFMediaType_Audio )
            streams.push_back( cursor );
        ++cursor;
    } while ( keepScanning );

    return streams;
}

//----------------------------------------------------------------------------------------------------------------------

template <class Source>
size_t calcStreamIndex( IMFSourceReader& reader, size_t stream )
{
    // The audio streams are indexed as follows:
    //
    // STREAM     INDEX
    // Mixdown    N
    // Stem 1     N-1
    // Stem 2     N-2
    // Stem 3     N-3
    // ...
    // Stem N     0

    auto indices = retrieveAudioStreamsIndices<Source>( reader );

    size_t minSize = stream + 1;
    if ( indices.size() < minSize )
    {
        throw std::runtime_error( boost::str(
            boost::format( "Could not open stream %u. Only %u streams found in file." ) % stream % indices.size() ) );
    }

    return indices[indices.size() - stream - 1];
}

} // namespace


//----------------------------------------------------------------------------------------------------------------------

template <class Source>
struct media_foundation_source<Source>::MfInitializer
{
    MfInitializer()
    {
        const auto com_ret = CoInitializeEx( nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE );
        m_coinitSucceeded  = SUCCEEDED( com_ret );
        // Don't treat already initialized COM as error
        if ( !m_coinitSucceeded && com_ret != RPC_E_CHANGED_MODE )
        {
            throw std::runtime_error( "Failed to initialize COM." );
        }
        if ( FAILED( MFStartup( MF_VERSION ) ) )
        {
            throw std::runtime_error( "Failed to startup WMF." );
        }
    }

    ~MfInitializer()
    {
        if ( SUCCEEDED( MFShutdown() ) && m_coinitSucceeded )
            CoUninitialize();
    }

    bool m_coinitSucceeded{false};
};

template <class Source>
struct media_foundation_source<Source>::MfBlock
{
    FrameRange           range;
    MfTypePtr<IMFSample> data;
};

template <class Source>
class media_foundation_source<Source>::MfBuffer
{
    using Buffer = MfTypePtr<IMFMediaBuffer>;
    using Sample = MfTypePtr<IMFSample>;

public:
    MfBuffer( std::unique_ptr<MfBlock> block )
    : m_validRange( block->range )
    {
        m_buffer = allocateNoThrow<Source>(
            [&block]( IMFMediaBuffer** p ) { return block->data->ConvertToContiguousBuffer( p ); } );

        DWORD size = 0;
        m_isLocked = m_buffer && SUCCEEDED( m_buffer->Lock( &m_data, nullptr, &size ) );
    }

    ~MfBuffer()
    {
        if ( m_isLocked )
            m_buffer->Unlock();
    }

    bool isLocked() const
    {
        return m_isLocked;
    }
    FrameRange frameRange() const
    {
        return m_validRange;
    }
    const BYTE* bufferData() const
    {
        return m_data;
    }

private:
    Buffer     m_buffer;
    bool       m_isLocked = false;
    FrameRange m_validRange{0, 0};
    BYTE*      m_data = nullptr;
};

const auto defaultOffset = 0;
const auto aacOfffset    = 2112;

//----------------------------------------------------------------------------------------------------------------------

template <class Source>
media_foundation_source<Source>::media_foundation_source( Source&&                             source,
                                                          audio::ifstream_info::container_type container,
                                                          size_t                               stream )
: m_initializer( new MfInitializer )
, m_byteStream( new SyncronousByteStream<Source>( std::move( source ) ) )
{
    m_reader = allocateOrThrow<Source>(
        [&]( IMFSourceReader** p ) { return MFCreateSourceReaderFromByteStream( m_byteStream, nullptr, p ); },
        "Could not open the audio file." );

    using codec_type     = audio::ifstream_info::codec_type;
    using container_type = audio::ifstream_info::container_type;

    if ( container_type::mp4 == container )
        m_readOffset = m_adjustedPos = aacOfffset;

    m_streamIndex = calcStreamIndex<Source>( *m_reader, stream );
    if ( FAILED( m_reader->SetStreamSelection( DWORD( MF_SOURCE_READER_ALL_STREAMS ), FALSE ) )
         || FAILED( m_reader->SetStreamSelection( DWORD( m_streamIndex ), TRUE ) ) )
    {
        throw std::runtime_error( "Could not select the audio stream." );
    }

    // Retrieve the native media attributes.

    auto nativeType = allocateOrThrow<Source>(
        [this]( IMFMediaType** p ) { return m_reader->GetNativeMediaType( DWORD( m_streamIndex ), 0, p ); },
        "Could not get native media type." );

    GUID id;
    if ( FAILED( nativeType->GetGUID( MF_MT_SUBTYPE, &id ) ) )
        throw std::runtime_error( "Could not read the media subtype." );


    if ( container_type::wma == container && boost::algorithm::any_of_equal( wmaFormats, id ) )
    {
        m_info.lossless( MFAudioFormat_WMAudio_Lossless == id );
        m_info.codec( codec_type::wma );
    }
    else if ( container_type::mp4 == container && MFAudioFormat_AAC == id )
    {
        m_info.lossless( false );
        m_info.codec( codec_type::aac );
    }
    else if ( container_type::mp4 == container && ( MFAudioFormat_ALAC == id || id.Data1 == 'alac' ) )
    {
        m_info.lossless( true );
        m_info.codec( codec_type::alac );
    }
    else if ( container_type::mp3 == container && boost::algorithm::any_of_equal( mp3Formats, id ) )
    {
        m_info.lossless( false );
        m_info.codec( codec_type::mp3 );
    }
    else
        throw std::runtime_error( "Could not retrieve codec info" );
    m_info.container( container );

    {
        UINT32 num_channels = 0;
        if ( FAILED( nativeType->GetUINT32( MF_MT_AUDIO_NUM_CHANNELS, &num_channels ) ) )
            throw std::runtime_error( "Could not read the number of channels." );

        if ( num_channels < 1u || num_channels > 255u )
            throw std::runtime_error( "Invalid number of channels: " + std::to_string( num_channels ) );

        m_info.num_channels( num_channels );
    }

    {
        UINT32 sample_rate = 0;
        if ( FAILED( nativeType->GetUINT32( MF_MT_AUDIO_SAMPLES_PER_SECOND, &sample_rate ) ) )
            throw std::runtime_error( "Could not read the sample rate." );

        if ( sample_rate < 1u )
            throw std::runtime_error( "Invalid sample rate: " + std::to_string( sample_rate ) );

        m_info.sample_rate( sample_rate );
    }

    {
        PROPVARIANT var;
        LONGLONG    length100ns = 0;
        if ( FAILED( m_reader->GetPresentationAttribute(
                 static_cast<DWORD>( MF_SOURCE_READER_MEDIASOURCE ), MF_PD_DURATION, &var ) )
             || FAILED( PropVariantToInt64( var, &length100ns ) ) || FAILED( PropVariantClear( &var ) ) )
        {
            throw std::runtime_error( "Could not read the track length." );
        }

        auto lengthFrames = time100nsToFrames<Source>( length100ns, m_info.sample_rate() );
        if ( lengthFrames < m_readOffset )
            throw std::runtime_error( "Empty File" );

        m_info.num_frames( size_t( lengthFrames - m_readOffset ) );
    }

    {
        UINT32 bitDepth = 0;
        auto   hr       = nativeType->GetUINT32( MF_MT_AUDIO_BITS_PER_SAMPLE, &bitDepth );
        if ( hr == MF_E_ATTRIBUTENOTFOUND )
            bitDepth = 16;
        else if ( FAILED( hr ) )
            throw std::runtime_error( "Could not read the bit depth." );

        m_info.format( {pcm::signed_integer, bitDepth, pcm::little_endian} );
    }

    {
        UINT32 byteRate = 0;
        if ( FAILED( nativeType->GetUINT32( MF_MT_AUDIO_AVG_BYTES_PER_SECOND, &byteRate ) ) )
            throw std::runtime_error( "Could not read the bit rate." );

        m_info.bit_rate( byteRate * 8 );
    }

    // Create a media type specifying uncompressed PCM audio and load it into the source reader. The reader will load
    // in turn the correct decoder.

    auto pcmType = allocateOrThrow<Source>( []( IMFMediaType** p ) { return MFCreateMediaType( p ); },
                                            "Could not create target media type." );

    auto bytesPerSecond = UINT32( m_info.bytes_per_frame() * m_info.sample_rate() );

    if ( FAILED( pcmType->SetGUID( MF_MT_MAJOR_TYPE, MFMediaType_Audio ) )
         || FAILED( pcmType->SetGUID( MF_MT_SUBTYPE, MFAudioFormat_PCM ) )
         || FAILED( pcmType->SetUINT32( MF_MT_AUDIO_BLOCK_ALIGNMENT, UINT32( m_info.bytes_per_frame() ) ) )
         || FAILED( pcmType->SetUINT32( MF_MT_AUDIO_AVG_BYTES_PER_SECOND, bytesPerSecond ) )
         || FAILED( pcmType->SetUINT32( MF_MT_ALL_SAMPLES_INDEPENDENT, TRUE ) )
         || FAILED( m_reader->SetCurrentMediaType( DWORD( m_streamIndex ), nullptr, pcmType.get() ) ) )
    {
        throw std::runtime_error( "Could not load uncompressed pcm decoder." );
    }

    if ( m_readOffset != 0 && !setPosition( *m_reader, framesTo100ns<Source>( m_readOffset, m_info.sample_rate() ) ) )
        throw std::runtime_error( "Could not reposition in media buffer." );
}

//----------------------------------------------------------------------------------------------------------------------

template <class Source>
media_foundation_source<Source>::~media_foundation_source()
{
    m_byteStream->Release();
}

//----------------------------------------------------------------------------------------------------------------------

template <class Source>
std::streampos media_foundation_source<Source>::seek( offset_type off, BOOST_IOS::seekdir way )
{
    assert( 0 == off % m_info.bytes_per_frame() );

    const auto frameSize  = m_info.bytes_per_frame();
    const auto begPos     = std::streampos( 0 );
    const auto endPos     = std::streampos( info().num_frames() );
    const auto nominalPos = absolute_position( m_nominalPos, begPos, endPos, off / frameSize, way );

    // The allowed range for the nominal offset  is [0, lengthInFrames[
    // The allowed range for the adjusted offset is [aacReadOffset, lengthInFrames + aacReadOffset[

    auto adjustedPos = nominalPos + m_readOffset;

    if ( m_nominalPos != nominalPos && seekInternal( adjustedPos ) )
    {
        m_nominalPos  = nominalPos;
        m_adjustedPos = adjustedPos;
    }

    return std::streampos( m_nominalPos * frameSize );
}

//----------------------------------------------------------------------------------------------------------------------

template <class Source>
std::streamsize media_foundation_source<Source>::read( char* dst, std::streamsize size )
{
    assert( 0 == size % m_info.bytes_per_frame() );

    auto frameSize = m_info.bytes_per_frame();
    auto endPos    = boost::algorithm::clamp<offset_type>( m_nominalPos + size / frameSize, 0, m_info.num_frames() );
    auto numFrames = endPos - m_nominalPos;

    if ( numFrames > 0 )
    {
        FrameRange toFill( m_adjustedPos, m_adjustedPos + numFrames );
        auto       remainingFirstPass = readFromBuffer( toFill, dst );

        FrameRangeSet remainingSecondPass;
        for ( const auto& range : remainingFirstPass )
        {
            auto offset = ( range.lower() - toFill.lower() ) * m_info.bytes_per_frame();
            remainingSecondPass += readFromFile( range, dst + offset );
        }

        for ( const auto& range : remainingSecondPass )
        {
            auto offset = ( range.lower() - toFill.lower() ) * frameSize;
            std::fill_n( dst + offset, boost::icl::size( range ) * frameSize, char( 0 ) );
        }
    }

    m_nominalPos += numFrames;
    m_adjustedPos += numFrames;
    return numFrames * frameSize;
}

//----------------------------------------------------------------------------------------------------------------------

template <class Source>
bool media_foundation_source<Source>::seekInternal( offset_type adjustedPos )
{
    // WMF applies a fade-in to the audio data fetched after a seek, which produces crackling. To avoid this, when
    // doing a seek we jump one block back of the target, then discard it in the following fetch.

    static const auto minValue     = s_defaultBlockSize + m_readOffset;
    bool              removeFadein = adjustedPos >= minValue;

    adjustedPos = removeFadein ? adjustedPos - s_defaultBlockSize : m_readOffset;
    if ( !setPosition( *m_reader, framesTo100ns<Source>( adjustedPos, m_info.sample_rate() ) ) )
        return false;

    m_buffer.reset();
    m_fadein = removeFadein;
    return true;
}

//----------------------------------------------------------------------------------------------------------------------

template <class Source>
auto media_foundation_source<Source>::readFromBuffer( const FrameRange& range, char* dst ) -> FrameRangeSet
{
    FrameRangeSet remaining;
    remaining += range;

    if ( !m_buffer )
        return remaining;

    auto bufferedRange = m_buffer->frameRange();
    auto intersection  = range & bufferedRange;

    using namespace boost::icl;

    if ( !is_empty( intersection ) )
    {
        auto src = m_buffer->bufferData() + ( intersection.lower() - bufferedRange.lower() ) * m_info.bytes_per_frame();
        auto dstBytes =
            reinterpret_cast<BYTE*>( dst ) + ( intersection.lower() - range.lower() ) * m_info.bytes_per_frame();

        std::copy_n( src, size( intersection ) * m_info.bytes_per_frame(), dstBytes );
        remaining -= intersection;
    }

    return remaining;
}

//----------------------------------------------------------------------------------------------------------------------

template <class Source>
auto media_foundation_source<Source>::readFromFile( const FrameRange& range, char* dst ) -> FrameRangeSet
{
    FrameRangeSet remaining;
    remaining += range;

    // We start by retrieving the leftmost data block with valid data.

    auto block = searchAndRetrieveBlock( range.lower() );
    if ( !block || !updateBuffer( std::move( block ) ) )
        return remaining;

    // We then fill out the remaining bits of the range by consuming the next data blocks. The output of
    // readFromBuffer() is a set which in this case contains only one element.

    remaining = readFromBuffer( range, dst );

    while ( !boost::icl::contains( m_buffer->frameRange(), range.upper() - 1 ) )
    {
        block = consumeBlock();
        if ( !block || !updateBuffer( std::move( block ) ) )
            return remaining;

        auto offset = ( remaining.begin()->lower() - range.lower() ) * m_info.bytes_per_frame();
        remaining   = readFromBuffer( *remaining.begin(), dst + offset );
    }

    return remaining;
}

//----------------------------------------------------------------------------------------------------------------------

template <class Source>
auto media_foundation_source<Source>::searchAndRetrieveBlock( offset_type target ) -> std::unique_ptr<MfBlock>
{
    // We start by discarding the first data block if it contains a fade-in.

    if ( !discardFadeinBlock() )
        return nullptr;

    // We then fetch the next block and check its timestamp to determine where we are in the audio stream, then move to
    // the target position.

    auto block = consumeBlock();
    if ( !block )
        return nullptr;

    const auto range = block->range;
    if ( boost::icl::contains( range, target ) )
        return block;

    block.reset();

    if ( target >= range.upper() )
        return searchForwardAndRetrieveBlock( target );
    else
        return searchBackwardAndRetrieveBlock( target, 0 );
}

//----------------------------------------------------------------------------------------------------------------------

template <class Source>
auto media_foundation_source<Source>::searchForwardAndRetrieveBlock( offset_type target ) -> std::unique_ptr<MfBlock>
{
    // If the target frame is ahead of the current position in the audio stream, we keep consuming data blocks until
    // we reach it. According to the WMF documentation, we are not going to overshoot.

    std::unique_ptr<MfBlock> block;
    do
    {
        block = consumeBlock();
    } while ( block && !boost::icl::contains( block->range, target ) );
    return block;
}

//----------------------------------------------------------------------------------------------------------------------

template <class Source>
auto media_foundation_source<Source>::searchBackwardAndRetrieveBlock( offset_type target, offset_type backstep )
    -> std::unique_ptr<MfBlock>
{
    // If the target frame is behind the current position in the audio stream, we seek backwards recursively with an
    // increasing step until we reach it. If we overshoot, we simply move forward by consuming the excess blocks.

    auto correctedTarget = target >= m_readOffset + backstep ? target - backstep : m_readOffset;
    if ( seekInternal( correctedTarget ) )
    {
        if ( !discardFadeinBlock() )
            return nullptr;

        auto block = consumeBlock();
        if ( !block )
            return nullptr;

        const auto range = block->range;
        if ( boost::icl::contains( range, target ) )
            return block;

        block.reset();

        if ( target >= range.upper() )
            return searchForwardAndRetrieveBlock( target );
        else
        {
            return correctedTarget > m_readOffset
                       ? searchBackwardAndRetrieveBlock( target, backstep + 2 * s_defaultBlockSize )
                       : nullptr;
        }
    }

    return nullptr;
}

namespace
{

//----------------------------------------------------------------------------------------------------------------------

bool checkErrors( const IMFSample* mfSample, DWORD flags )
{
    return ( flags & MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED ) == 0 && ( flags & MF_SOURCE_READERF_ENDOFSTREAM ) == 0
           && ( flags & MF_SOURCE_READERF_STREAMTICK ) == 0 && mfSample;
}

} // namespace

//----------------------------------------------------------------------------------------------------------------------

template <class Source>
auto media_foundation_source<Source>::consumeBlock() -> std::unique_ptr<MfBlock>
{
    DWORD    flags     = 0;
    LONGLONG timestamp = 0, duration = 0;
    auto     mfSample = allocateNoThrow<Source>( [this, &flags, &timestamp]( IMFSample** p ) {
        return m_reader->ReadSample( DWORD( m_streamIndex ), 0, nullptr, &flags, &timestamp, p );
    } );

    if ( !checkErrors( mfSample.get(), flags ) || FAILED( mfSample->GetSampleDuration( &duration ) ) )
        return nullptr;

    offset_type beg =
        m_buffer ? m_buffer->frameRange().upper() : time100nsToFrames<Source>( timestamp, m_info.sample_rate() );
    offset_type end = beg + time100nsToFrames<Source>( duration, m_info.sample_rate() );

    std::unique_ptr<MfBlock> block( new MfBlock{{beg, end}, std::move( mfSample )} );
    return block;
}

//----------------------------------------------------------------------------------------------------------------------

template <class Source>
bool media_foundation_source<Source>::updateBuffer( std::unique_ptr<MfBlock> block )
{
    auto buffer = boost::make_unique<MfBuffer>( std::move( block ) );
    if ( !buffer->isLocked() )
        return false;

    m_buffer.swap( buffer );
    return true;
}

//----------------------------------------------------------------------------------------------------------------------

template <class Source>
bool media_foundation_source<Source>::discardFadeinBlock()
{
    // If this is the first fetch after a seek we need to discard the first frame, as WMF applies a fade-in
    // effect to the first few samples that produces crackling when played back by Traktor.

    if ( m_fadein )
    {
        auto frame = consumeBlock();
        if ( !frame )
            return false;
        m_fadein = false;
    }

    return true;
}
