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

#include "core_audio_file_source.h"

#include <ni/media/iostreams/positioning.h>

#include <boost/algorithm/cxx11/any_of.hpp>
#include <boost/format.hpp>


namespace
{

const auto aacFormats = {kAudioFormatMPEG4AAC,
                         kAudioFormatMPEG4AAC_HE,
                         kAudioFormatMPEG4AAC_LD,
                         kAudioFormatMPEG4AAC_ELD,
                         kAudioFormatMPEG4AAC_ELD_SBR,
                         kAudioFormatMPEG4AAC_ELD_V2,
                         kAudioFormatMPEG4AAC_HE_V2,
                         kAudioFormatMPEG4AAC_Spatial};

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

//----------------------------------------------------------------------------------------------------------------------

audio::ifstream_info buildOutStreamInfo( ExtAudioFileRef& media, audio::ifstream_info::container_type container )
{
    using namespace pcm;
    audio::ifstream_info info;

    AudioStreamBasicDescription descriptor;
    UInt32                      size = sizeof( descriptor );
    if ( ExtAudioFileGetProperty( media, kExtAudioFileProperty_FileDataFormat, &size, &descriptor ) != noErr )
        throw std::runtime_error( "Could not retrieve the audio format." );

    info.format( make_format<floating_point, _32bit, little_endian>() );
    info.sample_rate( descriptor.mSampleRate );
    info.num_channels( descriptor.mChannelsPerFrame );

    {
        SInt64 numFrames = 0;
        UInt32 size      = sizeof( numFrames );
        if ( ExtAudioFileGetProperty( media, kExtAudioFileProperty_FileLengthFrames, &size, &numFrames ) != noErr )
            throw std::runtime_error( "Could not read the track length." );

        info.num_frames( size_t( numFrames ) );
    }

    {
        AudioFileID fileId;
        UInt32      size = sizeof( fileId );
        if ( ExtAudioFileGetProperty( media, kExtAudioFileProperty_AudioFile, &size, &fileId ) != noErr )
            throw std::runtime_error( "Could not retrieve the file id." );

        UInt32 fileFormat = 0;
        size              = sizeof( fileFormat );
        if ( AudioFileGetProperty( fileId, kAudioFilePropertyFileFormat, &size, &fileFormat ) != noErr )
            throw std::runtime_error( "Could not read file format" );

        using codec_type     = audio::ifstream_info::codec_type;
        using container_type = audio::ifstream_info::container_type;

        if ( container_type::mp3 == container && kAudioFileMP3Type == fileFormat )
        {
            info.codec( codec_type::mp3 );
            info.lossless( false );
        }
        else if ( container_type::mp4 == container
                  && ( kAudioFileMPEG4Type == fileFormat || kAudioFileM4AType == fileFormat ) )
        {
            info.codec( mp4_codec_type( descriptor.mFormatID ) );
            info.lossless( info.codec() == codec_type::alac );
        }
        else
            throw std::runtime_error( "Unsupported file format" );

        info.container( container );
    }

    return info;
}

} // namespace anonymous

//----------------------------------------------------------------------------------------------------------------------

core_audio_file_source::core_audio_file_source( const std::string&                   path,
                                                audio::ifstream_info::container_type container,
                                                size_t                               stream )
{
    using CFURLAutoRef = std::unique_ptr<std::pointer_traits<CFURLRef>::element_type, std::function<void( CFTypeRef )>>;
    auto url           = CFURLAutoRef( CFURLCreateFromFileSystemRepresentation(
                                 nullptr, reinterpret_cast<const UInt8*>( path.c_str() ), path.size(), false ),
                             &CFRelease );
    if ( ExtAudioFileOpenURL( url.get(), &m_media ) != noErr )
        throw std::runtime_error( "Could not open the audio file." );

    AudioFileID fileId;
    UInt32      size = sizeof( fileId );
    if ( ExtAudioFileGetProperty( m_media, kExtAudioFileProperty_AudioFile, &size, &fileId ) != noErr )
        throw std::runtime_error( "Could not retrieve the file id." );


    if ( container == audio::ifstream_info::container_type::mp4 )
    {
        auto index = calcStreamIndex( fileId, stream );
        if ( AudioFileSetProperty( fileId, 'uatk', sizeof( index ), &index ) != noErr )
            throw std::runtime_error( "Could not select the audio stream." );
    }
    else if ( stream != 0 )
        throw std::runtime_error( "Invalid stream" );

    auto outStreamInfo = buildOutStreamInfo( m_media, container );

    AudioStreamBasicDescription description{.mFormatID         = kAudioFormatLinearPCM,
                                            .mChannelsPerFrame = UInt32( outStreamInfo.num_channels() ),
                                            .mSampleRate       = Float64( outStreamInfo.sample_rate() ),
                                            .mFormatFlags      = kAudioFormatFlagIsFloat,
                                            .mFramesPerPacket  = 1,
                                            .mBitsPerChannel   = UInt32( outStreamInfo.bits_per_sample() ),
                                            .mBytesPerPacket   = UInt32( outStreamInfo.bytes_per_frame() ),
                                            .mBytesPerFrame    = UInt32( outStreamInfo.bytes_per_frame() )};

    if ( ExtAudioFileSetProperty( m_media, kExtAudioFileProperty_ClientDataFormat, sizeof( description ), &description )
         != noErr )
    {
        throw std::runtime_error( "Could not set the output format." );
    }

    m_info = outStreamInfo;
}

//----------------------------------------------------------------------------------------------------------------------

core_audio_file_source::~core_audio_file_source()
{
    ExtAudioFileDispose( m_media );
}

//----------------------------------------------------------------------------------------------------------------------

audio::ifstream_info core_audio_file_source::info() const
{
    return m_info;
}

//----------------------------------------------------------------------------------------------------------------------

std::streampos core_audio_file_source::seek( offset_type off, BOOST_IOS::seekdir way )
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

std::streamsize core_audio_file_source::read( char* dst, std::streamsize size )
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
