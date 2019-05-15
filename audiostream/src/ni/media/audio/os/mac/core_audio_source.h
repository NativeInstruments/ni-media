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

#include <ni/media/audio/fourcc.h>
#include <ni/media/audio/ifstream_info.h>
#include <ni/media/iostreams/positioning.h>

#include <boost/algorithm/cxx11/any_of.hpp>
#include <boost/format.hpp>

#include <AudioToolbox/ExtendedAudioFile.h>

#include <cassert>
#include <stdexcept>
#include <string>

template <class Source>
class core_audio_source
{
    using offset_type = boost::iostreams::stream_offset;

public:
    using info_type = audio::ifstream_info;

    core_audio_source( Source&& source, audio::ifstream_info::container_type container, size_t stream = 0 );
    ~core_audio_source();

    audio::ifstream_info info() const;

    std::streampos  seek( offset_type, BOOST_IOS::seekdir );
    std::streamsize read( char*, std::streamsize );

    struct ClientData
    {
        ClientData( Source&& source_ )
        : source( std::move( source_ ) )
        {
            auto pos = static_cast<SInt64>( source.seek( boost::iostreams::stream_offset( 0 ), BOOST_IOS::end ) );
            position = size = pos > 0 ? pos : 0;
        }
        Source source;
        SInt64 position = 0;
        SInt64 size     = 0;
    };

private:
    void setupInfo( audio::ifstream_info::container_type container, size_t stream, const AudioFileID& fileId );

    ClientData           m_clientData;
    ExtAudioFileRef      m_media;
    audio::ifstream_info m_info;
    std::streampos       m_pos = 0;
};


namespace
{

const auto alacFormats = {kAudioFormatAppleLossless};

const auto aacFormats = {kAudioFormatMPEG4AAC,
                         kAudioFormatMPEG4AAC_HE,
                         kAudioFormatMPEG4AAC_LD,
                         kAudioFormatMPEG4AAC_ELD,
                         kAudioFormatMPEG4AAC_ELD_SBR,
                         kAudioFormatMPEG4AAC_ELD_V2,
                         kAudioFormatMPEG4AAC_HE_V2,
                         kAudioFormatMPEG4AAC_Spatial};

const auto mp3Formats = {kAudioFormatMPEGLayer3};


bool isMp4Format( const AudioFormatID& id )
{
    return boost::algorithm::any_of_equal( aacFormats, id ) || boost::algorithm::any_of_equal( alacFormats, id );
}

bool isMp3Format( const AudioFormatID& id )
{
    return boost::algorithm::any_of_equal( mp3Formats, id );
}

bool isSupportedFormat( const AudioFormatID& id )
{
    return isMp3Format( id ) || isMp4Format( id );
}

//----------------------------------------------------------------------------------------------------------------------

size_t calcStreamIndex( AudioFileID fileId, size_t stream )
{
    // Get the number of available streams within the container file.
    UInt32 numStreams = 0;
    UInt32 size       = sizeof( fileId );
    if ( AudioFileGetProperty( fileId, 'atct', &size, &numStreams ) != noErr )
        throw std::runtime_error( "Failed to retrieve stream count." );

    // The audio streams are indexed as follows:
    //
    // STREAM     INDEX
    // Mixdown    0
    // Stem 1     1
    // Stem 2     2
    // Stem 3     3
    // ...
    // Stem N     N

    size_t minSize = stream + 1;
    if ( minSize > numStreams )
    {
        throw std::runtime_error( boost::str(
            boost::format( "Could not open stream %u. Only %u streams found in file." ) % stream % numStreams ) );
    }

    return stream;
}

//----------------------------------------------------------------------------------------------------------------------

auto mp4_codec_type( UInt32 codecId )
{
    using codec_type = audio::ifstream_info::codec_type;
    if ( codecId == kAudioFormatAppleLossless )
        return codec_type::alac;
    if ( boost::algorithm::any_of_equal( aacFormats, codecId ) )
        return codec_type::aac;
    // just assume aac
    return codec_type::aac;
}

AudioFormatFlags formatToAudioFormatFlags( const pcm::format& format )
{
    AudioFormatFlags flags = 0;

    if ( format.endian() == pcm::big_endian )
        flags |= kAudioFormatFlagIsBigEndian;

    if ( format.number() == pcm::floating_point )
        flags |= kAudioFormatFlagIsFloat;

    if ( format.number() == pcm::signed_integer )
        flags |= kAudioFormatFlagIsSignedInteger;

    return flags;
}
//----------------------------------------------------------------------------------------------------------------------

audio::ifstream_info buildOutStreamInfo( ExtAudioFileRef&                     media,
                                         const AudioFileID&                   fileId,
                                         audio::ifstream_info::container_type container )
{
    using namespace pcm;
    audio::ifstream_info info;

    AudioStreamBasicDescription descriptor;
    UInt32                      size = sizeof( descriptor );
    if ( ExtAudioFileGetProperty( media, kExtAudioFileProperty_FileDataFormat, &size, &descriptor ) != noErr )
        throw std::runtime_error( "Could not retrieve the audio format." );

    if ( !isSupportedFormat( descriptor.mFormatID ) )
        throw std::runtime_error( "Unsupported format id: " + fourcc_to_string( descriptor.mFormatID ) );

    if ( descriptor.mSampleRate <= 0.0 )
        throw std::runtime_error( "Invalid sample rate: " + std::to_string( descriptor.mSampleRate ) );
    info.sample_rate( size_t( descriptor.mSampleRate ) );

    if ( descriptor.mChannelsPerFrame < 1u || descriptor.mChannelsPerFrame > 255u )
        throw std::runtime_error( "Invalid number of channels: " + std::to_string( descriptor.mChannelsPerFrame ) );
    info.num_channels( size_t( descriptor.mChannelsPerFrame ) );

    {
        SInt64 numFrames = 0;
        UInt32 size      = sizeof( numFrames );
        if ( ExtAudioFileGetProperty( media, kExtAudioFileProperty_FileLengthFrames, &size, &numFrames ) != noErr )
            throw std::runtime_error( "Could not read the track length." );

        if ( numFrames < 1 )
            throw std::runtime_error( "Empty File" );
        info.num_frames( size_t( numFrames ) );
    }

    {
        UInt32 fileFormat = 0;
        size              = sizeof( fileFormat );
        if ( AudioFileGetProperty( fileId, kAudioFilePropertyFileFormat, &size, &fileFormat ) != noErr )
            throw std::runtime_error( "Could not read file format" );

        using codec_type     = audio::ifstream_info::codec_type;
        using container_type = audio::ifstream_info::container_type;

        // we set the default format to 32bit float
        info.format( make_format<floating_point, _32bit, native_endian>() );

        if ( container_type::mp3 == container && kAudioFileMP3Type == fileFormat )
        {
            info.codec( codec_type::mp3 );
            info.lossless( false );
        }
        else if ( container_type::mp4 == container
                  && ( kAudioFileMPEG4Type == fileFormat || kAudioFileM4AType == fileFormat ) )
        {
            info.codec( mp4_codec_type( descriptor.mFormatID ) );
            if ( info.codec() == codec_type::alac )
            {
                if ( descriptor.mFormatFlags == kAppleLosslessFormatFlag_16BitSourceData )
                    info.format( make_format<signed_integer, _16bit, native_endian>() );
                else if ( descriptor.mFormatFlags == kAppleLosslessFormatFlag_20BitSourceData )
                    info.format( make_format<signed_integer, _24bit, native_endian>() );
                else if ( descriptor.mFormatFlags == kAppleLosslessFormatFlag_24BitSourceData )
                    info.format( make_format<signed_integer, _24bit, native_endian>() );
                else if ( descriptor.mFormatFlags == kAppleLosslessFormatFlag_32BitSourceData )
                    info.format( make_format<signed_integer, _32bit, native_endian>() );

                info.lossless( true );
            }
            else
            {
                info.lossless( false );
            }
        }
        else
            throw std::runtime_error( "Unsupported file format" );

        info.container( container );
    }

    return info;
}


//----------------------------------------------------------------------------------------------------------------------
template <class Source>
OSStatus AudioFileReadProc(
    void* inClientData, SInt64 inPosition, UInt32 requestCount, void* buffer, UInt32* actualCount )
{
    auto clientData = static_cast<typename core_audio_source<Source>::ClientData*>( inClientData );
    if ( clientData->position != inPosition )
    {
        clientData->source.seek( boost::iostreams::stream_offset( inPosition ), BOOST_IOS::beg );
        clientData->position = inPosition;
    }

    auto numRead = clientData->source.read( static_cast<char*>( buffer ), requestCount );
    *actualCount = numRead > 0 ? static_cast<UInt32>( numRead ) : 0;
    clientData->position += *actualCount;
    return noErr;
}

//----------------------------------------------------------------------------------------------------------------------
template <class Source>
SInt64 AudioFileGetSizeProc( void* inClientData )
{
    auto clientData = static_cast<typename core_audio_source<Source>::ClientData*>( inClientData );
    return clientData->size;
}

} // namespace

//----------------------------------------------------------------------------------------------------------------------
template <class Source>
core_audio_source<Source>::core_audio_source( Source&&                             source,
                                              audio::ifstream_info::container_type container,
                                              size_t                               stream )
: m_clientData( std::move( source ) )
{
    auto audioFileTypeHint =
        container == audio::ifstream_info::container_type::mp3 ? kAudioFileMP3Type : kAudioFileMPEG4Type;

    AudioFileID fileId;

    if ( AudioFileOpenWithCallbacks( &m_clientData,
                                     AudioFileReadProc<Source>,
                                     nullptr,
                                     AudioFileGetSizeProc<Source>,
                                     nullptr,
                                     audioFileTypeHint,
                                     &fileId )
         != noErr )
        throw std::runtime_error( "Could not create the audio file stream." );

    if ( ExtAudioFileWrapAudioFileID( fileId, false, &m_media ) != noErr )
        throw std::runtime_error( "Could not wrap the audio stream." );

    setupInfo( container, stream, fileId );
}

//----------------------------------------------------------------------------------------------------------------------
template <class Source>
core_audio_source<Source>::~core_audio_source()
{
    ExtAudioFileDispose( m_media );
}

//----------------------------------------------------------------------------------------------------------------------
template <class Source>
void core_audio_source<Source>::setupInfo( audio::ifstream_info::container_type container,
                                           size_t                               stream,
                                           const AudioFileID&                   fileId )
{
    if ( container == audio::ifstream_info::container_type::mp4 )
    {
        auto index = calcStreamIndex( fileId, stream );
        if ( AudioFileSetProperty( fileId, 'uatk', sizeof( index ), &index ) != noErr )
            throw std::runtime_error( "Could not select the audio stream." );
    }
    else if ( stream != 0 )
        throw std::runtime_error( "Invalid stream" );

    auto info = buildOutStreamInfo( m_media, fileId, container );

    AudioStreamBasicDescription description{.mFormatID         = kAudioFormatLinearPCM,
                                            .mChannelsPerFrame = UInt32( info.num_channels() ),
                                            .mSampleRate       = Float64( info.sample_rate() ),
                                            .mFormatFlags      = formatToAudioFormatFlags( info.format() ),
                                            .mFramesPerPacket  = 1,
                                            .mBitsPerChannel   = UInt32( info.bits_per_sample() ),
                                            .mBytesPerPacket   = UInt32( info.bytes_per_frame() ),
                                            .mBytesPerFrame    = UInt32( info.bytes_per_frame() )};

    if ( ExtAudioFileSetProperty( m_media, kExtAudioFileProperty_ClientDataFormat, sizeof( description ), &description )
         != noErr )
    {
        throw std::runtime_error( "Could not set the output format." );
    }

    m_info = info;
}

//----------------------------------------------------------------------------------------------------------------------
template <class Source>
audio::ifstream_info core_audio_source<Source>::info() const
{
    return m_info;
}

//----------------------------------------------------------------------------------------------------------------------
template <class Source>
std::streampos core_audio_source<Source>::seek( offset_type off, BOOST_IOS::seekdir way )
{
    const auto beg = std::streampos( 0 );
    const auto end = std::streampos( info().num_bytes() );
    const auto pos = absolute_position( m_pos, beg, end, off, way );

    assert( 0 == pos % m_info.bytes_per_frame() );

    if ( m_pos != pos && ExtAudioFileSeek( m_media, pos / m_info.bytes_per_frame() ) == noErr )
        m_pos = pos;

    return m_pos;
}

//----------------------------------------------------------------------------------------------------------------------
template <class Source>
std::streamsize core_audio_source<Source>::read( char* dst, std::streamsize size )
{
    assert( 0 == size % m_info.bytes_per_frame() );

    const auto end = std::streampos( info().num_bytes() );
    if ( size <= 0 || m_pos >= end )
        return 0;

    size = std::min( size, std::streamsize( end - m_pos ) );

    AudioBufferList buffer{.mNumberBuffers = 1,
                           .mBuffers[0]    = {.mNumberChannels = UInt32( m_info.num_channels() ),
                                           .mDataByteSize   = UInt32( size ),
                                           .mData           = static_cast<void*>( dst )}};

    auto num_frames = UInt32( size / m_info.bytes_per_frame() );
    if ( noErr != ExtAudioFileRead( m_media, &num_frames, &buffer ) )
        return 0;

    auto num_bytes = std::streamsize( num_frames * m_info.bytes_per_frame() );
    m_pos += num_bytes;
    return num_bytes;
}
