//
// Copyright (c) 2017-2023 Native Instruments GmbH, Berlin
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

#include "avassetreader_source.h"

#include <ni/media/iostreams/positioning.h>

#include <AVFoundation/AVFoundation.h>

#include <boost/algorithm/clamp.hpp>
#include <boost/format.hpp>
#include <boost/optional.hpp>

#include <algorithm>
#include <cstring>

//----------------------------------------------------------------------------------------------------------------------

class avassetreader_source::Impl
{
public:
    Impl( const std::string& avAssetUrl,
          size_t             streamIndex,
          size_t             seekPositionFrames,
          size_t             beginOffsetFrames,
          bool               enablePreciseTiming );
    ~Impl();

    std::streamsize read( char*, std::streamsize );

private:
    bool getNextSampleBufferIfNeeded();

public:
    std::string          m_avAssetUrl;
    size_t               m_streamIndex;
    offset_type          m_framePos = 0;
    audio::ifstream_info m_info;

private:
    AVAssetReader*            m_reader      = nullptr;
    AVAssetReaderTrackOutput* m_trackOutput = nullptr;

    class SampleBuffer
    {
    public:
        SampleBuffer( CMSampleBufferRef );
        ~SampleBuffer();

        std::streamsize copy( char*, std::streamsize );

        size_t remainingBytes() const;

    private:
        CMSampleBufferRef m_sampleBuffer = nullptr;
        CMBlockBufferRef  m_blockBuffer  = nullptr;
        size_t            m_consumed     = 0;
        size_t            m_length       = 0;
    };

    boost::optional< SampleBuffer > m_currentSampleBuffer;
    bool                            m_sourceExhausted = false;
};

//----------------------------------------------------------------------------------------------------------------------

namespace
{

audio::ifstream_info buildOutStreamInfo( AVAssetReaderTrackOutput* output, size_t beginOffsetFrames )
{
    using namespace pcm;
    audio::ifstream_info info;

    NSDictionary< NSString*, id >* settings = output.outputSettings;
    if ( settings == nil )
    {
        throw std::runtime_error( "Could not retrieve the audio format." );
    }

    info.format( make_format< floating_point, _32bit, little_endian >() );

    size_t sampleRate = [[settings objectForKey:AVSampleRateKey] intValue];
    if ( sampleRate == 0 )
    {
        throw std::runtime_error( "Could not retreive the sample rate." );
    }
    info.sample_rate( sampleRate );

    info.num_channels( [[settings objectForKey:AVNumberOfChannelsKey] intValue] );

    // calculate duration in frames
    size_t numFrames = 0;
    // if the timescale is in samples, then we can use the value directly
    if ( static_cast< size_t >( output.track.asset.duration.timescale ) == sampleRate )
    {
        numFrames = static_cast< size_t >( output.track.asset.duration.value );
    }
    else
    {
        double seconds = output.track.asset.duration.value / output.track.asset.duration.timescale;
        numFrames      = static_cast< size_t >( seconds * sampleRate );
    }

    if ( beginOffsetFrames > numFrames )
    {
        numFrames = 0;
    }
    else
    {
        numFrames -= beginOffsetFrames;
    }
    info.num_frames( numFrames );

    return info;
}

inline bool isMp3( const std::string& avAssetUrl )
{
    return avAssetUrl.find( ".mp3" ) != std::string::npos;
}

constexpr size_t skip_mp3_leader_frames = 576;

} // namespace anonymous

//----------------------------------------------------------------------------------------------------------------------

std::unique_ptr< avassetreader_source::Impl > avassetreader_source::create_impl( const std::string& avAssetUrl,
                                                                                 size_t             streamIndex,
                                                                                 size_t             startTimeFrames )
{
    bool mp3 = isMp3( avAssetUrl );
    
    if ( !m_trimOffsetDetermined )
    {
        m_trimOffsetFrames = 0;
        if ( mp3 )
        {
            auto precise    = std::make_unique< avassetreader_source::Impl >( avAssetUrl, streamIndex, 0, 0, true );
            auto notPrecise = std::make_unique< avassetreader_source::Impl >( avAssetUrl, streamIndex, 0, 0, false );

            // 128 mp3 granules should get us to real content (~2s) in the case of digital silence in the content
            const size_t max_comparison_granules = 128;
            const size_t max_comparison_length_bytes =
                skip_mp3_leader_frames * precise->m_info.bytes_per_frame() * max_comparison_granules;
            size_t examined_length_bytes = 0;

            const size_t        previewBufferSizeBytes = skip_mp3_leader_frames * precise->m_info.bytes_per_frame();
            std::vector< char > preciseData, notPreciseData;
            while ( examined_length_bytes < max_comparison_length_bytes )
            {
                preciseData.clear();
                preciseData.resize( previewBufferSizeBytes, 0 );
                notPreciseData.clear();
                notPreciseData.resize( previewBufferSizeBytes, 0 );

                std::streampos examined = precise->read( preciseData.data(), previewBufferSizeBytes );
                notPrecise->read( notPreciseData.data(), previewBufferSizeBytes );
                examined_length_bytes += examined;

                if ( std::memcmp( preciseData.data(), notPreciseData.data(), previewBufferSizeBytes ) != 0 )
                {
                    m_trimOffsetFrames = skip_mp3_leader_frames;
                    break;
                }
            }
        }
        m_trimOffsetDetermined = true;
    }
    
    // Seeking MP3s on iOS will be very inaccurate without precise timing, so it is always enabled for MP3s
    bool enablePreciseTiming = m_trimOffsetFrames == 0 || mp3;

    return std::make_unique< avassetreader_source::Impl >(
        avAssetUrl, streamIndex, startTimeFrames, m_trimOffsetFrames, enablePreciseTiming );
}

//----------------------------------------------------------------------------------------------------------------------

avassetreader_source::avassetreader_source() = default;

avassetreader_source::avassetreader_source( const std::string& avAssetUrl, size_t stream )
{
    open( avAssetUrl, stream );
}

avassetreader_source::Impl::Impl( const std::string& avAssetUrl,
                                  size_t             streamIndex,
                                  size_t             seekPositionFrames,
                                  size_t             beginOffsetFrames,
                                  bool               enablePreciseTiming )
: m_avAssetUrl( avAssetUrl )
, m_streamIndex( streamIndex )
{
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
    try
    {
        @try
        {
            NSDictionary* songOptions = @{
                AVURLAssetPreferPreciseDurationAndTimingKey :
                    [NSNumber numberWithBool:( enablePreciseTiming ? YES : NO )]
            };
            NSURL*      url   = [NSURL URLWithString:[NSString stringWithUTF8String:avAssetUrl.c_str()]];
            AVURLAsset* asset = [AVURLAsset URLAssetWithURL:url options:songOptions];

            if ( [asset hasProtectedContent] )
            {
                throw std::runtime_error( boost::str(
                    boost::format( "This track is DRM protected and cannot be loaded: %s." ) % avAssetUrl ) );
            }

            NSError*       error  = nil;
            AVAssetReader* reader = [[[AVAssetReader alloc] initWithAsset:asset error:&error] autorelease];
            if ( error != nil )
            {
                throw std::runtime_error( boost::str( boost::format( "Could not open url %s. %s." ) % avAssetUrl %
                                                      [[error description] UTF8String] ) );
            }

            NSArray< AVAssetTrack* >* audioTracks = [asset tracksWithMediaType:AVMediaTypeAudio];
            if ( streamIndex >= audioTracks.count )
            {
                throw std::runtime_error( boost::str(
                    boost::format( "Asset %s did not contain an audio stream matching the requested index %u" )
                    % avAssetUrl % streamIndex ) );
            }

            AVAssetTrack*               track = [audioTracks objectAtIndex:streamIndex];
            CMAudioFormatDescriptionRef trackDescription =
                static_cast< CMAudioFormatDescriptionRef >( [track.formatDescriptions firstObject] );
            if ( trackDescription == nullptr )
            {
                throw std::runtime_error(
                    boost::str( boost::format( "Could not retrieve audio format description from %s for track %u" )
                                % avAssetUrl % streamIndex ) );
            }

            const AudioStreamBasicDescription* trackAsbd =
                CMAudioFormatDescriptionGetStreamBasicDescription( trackDescription );
            if ( trackAsbd == nullptr )
            {
                throw std::runtime_error( boost::str(
                    boost::format( "Could not retrieve audio stream basic description from %s for track %u" )
                    % avAssetUrl % streamIndex ) );
            }

            NSDictionary* outputSettingsDict = @{
                AVFormatIDKey : @( kAudioFormatLinearPCM ),
                AVLinearPCMBitDepthKey : @32,
                AVLinearPCMIsBigEndianKey : @NO,
                AVLinearPCMIsFloatKey : @YES,
                AVLinearPCMIsNonInterleaved : @NO,
                AVSampleRateKey : @( trackAsbd->mSampleRate ),
                AVNumberOfChannelsKey : @( trackAsbd->mChannelsPerFrame )
            };
            AVAssetReaderTrackOutput* output =
                [[[AVAssetReaderTrackOutput alloc] initWithTrack:track outputSettings:outputSettingsDict] autorelease];
            if ( ![reader canAddOutput:output] )
            {
                throw std::runtime_error(
                    boost::str( boost::format( "Failed to attach track output to asset %s" ) % avAssetUrl ) );
            }
            output.alwaysCopiesSampleData = NO;
            [reader addOutput:output]; // the reader takes ownership of the output

            m_info = buildOutStreamInfo( output, beginOffsetFrames );

            // calculate start position
            size_t endPos              = m_info.num_frames() + beginOffsetFrames - 1;
            size_t startTimeFrames     = boost::algorithm::clamp( seekPositionFrames + beginOffsetFrames, 0, endPos );
            m_framePos                 = startTimeFrames;
            CMTime      positionInTime = CMTimeMake( startTimeFrames, static_cast< int32_t >( m_info.sample_rate() ) );
            CMTimeRange seekToRange    = CMTimeRangeMake( positionInTime, kCMTimePositiveInfinity );
            reader.timeRange           = seekToRange;

            // start
            if ( ![reader startReading] )
            {
                throw std::runtime_error( boost::str( boost::format( "Failed to start reading asset %s. Error: %s" )
                                                      % avAssetUrl % [[reader.error description] UTF8String] ) );
            }

            m_reader      = [reader retain];
            m_trackOutput = [output retain];
        }
        @catch ( NSException* ex )
        {
            throw std::runtime_error( boost::str( boost::format( "Failure creating decoder for asset %s. %s" )
                                                  % avAssetUrl % [ex.reason UTF8String] ) );
        }
        @finally
        {
            [pool drain];
        }
    }
    catch ( const std::exception& )
    {
        throw;
    }
}

//----------------------------------------------------------------------------------------------------------------------

avassetreader_source::avassetreader_source( avassetreader_source&& ) = default;

//----------------------------------------------------------------------------------------------------------------------

avassetreader_source::~avassetreader_source() = default;

avassetreader_source::Impl::~Impl()
{
    [m_reader cancelReading];
    [m_trackOutput release];
    [m_reader release];
}

//----------------------------------------------------------------------------------------------------------------------

void avassetreader_source::open( const std::string& avAssetUrl, size_t stream )
{
    m_trimOffsetDetermined = false;
    m_impl                 = create_impl( avAssetUrl, stream, 0 );
}

//----------------------------------------------------------------------------------------------------------------------

void avassetreader_source::close()
{
    m_impl.reset();
}

//----------------------------------------------------------------------------------------------------------------------

bool avassetreader_source::is_open() const
{
    return m_impl != nullptr;
}

//----------------------------------------------------------------------------------------------------------------------

audio::ifstream_info avassetreader_source::info() const
{
    return m_impl ? m_impl->m_info : audio::ifstream_info();
}

//----------------------------------------------------------------------------------------------------------------------

std::streampos avassetreader_source::seek( offset_type offset, BOOST_IOS::seekdir way )
{
    if ( m_impl == nullptr )
    {
        return std::streampos( 0 );
    }

    size_t frameSize = m_impl->m_info.bytes_per_frame();

    // optimization to avoid rebuilding
    if ( offset == 0 && way == BOOST_IOS::seekdir::cur )
    {
        return std::streampos( m_impl->m_framePos * frameSize );
    }

    // units are frames
    std::streampos beg         = 0;
    std::streampos cur         = m_impl->m_framePos;
    std::streampos end         = m_impl->m_info.num_frames();
    offset_type    frameOffset = static_cast< size_t >( offset / frameSize );

    std::streampos seekToFrames = absolute_position( cur, beg, end, frameOffset, way );

    // AVAssetReader cannot be seeked once we have decoded anything
    m_impl = create_impl( m_impl->m_avAssetUrl, m_impl->m_streamIndex, static_cast< size_t >( seekToFrames ) );

    return boost::iostreams::stream_offset_to_streamoff( seekToFrames * frameSize );
}

//----------------------------------------------------------------------------------------------------------------------

bool avassetreader_source::Impl::getNextSampleBufferIfNeeded()
{
    if ( m_currentSampleBuffer != boost::none && m_currentSampleBuffer->remainingBytes() > 0 )
    {
        return true;
    }

    CMSampleBufferRef sampleBufferRef = [m_trackOutput copyNextSampleBuffer];
    if ( sampleBufferRef != nullptr )
    {
        m_currentSampleBuffer.emplace( sampleBufferRef );
        return true;
    }

    m_sourceExhausted = true;
    m_currentSampleBuffer.reset();
    return false;
}

//----------------------------------------------------------------------------------------------------------------------

std::streamsize avassetreader_source::read( char* dst, std::streamsize size )
{
    return m_impl ? m_impl->read( dst, size ) : std::streamsize( 0 );
}

std::streamsize avassetreader_source::Impl::read( char* dst, std::streamsize size )
{
    @autoreleasepool
    {
        std::streamsize numCharsRead = 0;
        while ( getNextSampleBufferIfNeeded() && ( numCharsRead < size ) )
        {
            numCharsRead += m_currentSampleBuffer->copy( dst + numCharsRead, size - numCharsRead );
        }

        // silence underrun
        if ( numCharsRead < size )
        {
            std::fill( dst + numCharsRead, dst + size, char( 0 ) );
        }

        // advance the stream even if we have exhausted the source
        auto frameSize = m_info.bytes_per_frame();
        if ( m_sourceExhausted )
        {
            // frames
            offset_type totalFrames = static_cast< offset_type >( m_info.num_frames() );
            if ( totalFrames > m_framePos )
            {
                offset_type remain = totalFrames - m_framePos - ( numCharsRead / frameSize );
                // bytes
                remain *= frameSize;
                offset_type advance = std::min( remain, static_cast< offset_type >( size - numCharsRead ) );
                numCharsRead += advance;
            }
        }

        m_framePos += numCharsRead / frameSize;

        return numCharsRead;
    }
}

//----------------------------------------------------------------------------------------------------------------------
// avassetreader_source::Impl::SampleBuffer
//----------------------------------------------------------------------------------------------------------------------

avassetreader_source::Impl::SampleBuffer::SampleBuffer( CMSampleBufferRef buffer )
: m_sampleBuffer( buffer )
, m_consumed( 0 )
{
    if ( m_sampleBuffer == nullptr )
    {
        throw std::runtime_error( "buffer ref cannot be null" );
    }

    m_blockBuffer = CMSampleBufferGetDataBuffer( buffer );
    m_length      = CMBlockBufferGetDataLength( m_blockBuffer );
}

avassetreader_source::Impl::SampleBuffer::~SampleBuffer()
{
    CMSampleBufferInvalidate( m_sampleBuffer );
    CFRelease( m_sampleBuffer );
}

std::streamsize avassetreader_source::Impl::SampleBuffer::copy( char* outputBuffer, std::streamsize requested )
{
    size_t available = remainingBytes();
    size_t toRead    = std::min( available, static_cast< size_t >( requested ) );
    CMBlockBufferCopyDataBytes( m_blockBuffer, m_consumed, toRead, outputBuffer );
    m_consumed += toRead;
    return std::streamsize( toRead );
}

size_t avassetreader_source::Impl::SampleBuffer::remainingBytes() const
{
    return m_length - m_consumed;
}
