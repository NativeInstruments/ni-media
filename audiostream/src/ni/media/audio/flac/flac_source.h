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

#pragma once

#include <ni/media/audio/ifstream_info.h>

#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/positioning.hpp>

#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

#define FLAC__NO_DLL
#include <FLAC++/decoder.h>

//----------------------------------------------------------------------------------------------------------------------
// Implementation class

template <typename Source>
class flac_source
{

public:
    using char_type = char;
    struct category : boost::iostreams::input_seekable, boost::iostreams::device_tag
    {
    };

    using info_type = audio::ifstream_info;

    flac_source( Source&& );
    std::streamsize read( char_type* s, std::streamsize n );
    std::streampos  seek( boost::iostreams::stream_offset off, BOOST_IOS::seekdir way );

    audio::ifstream_info info() const
    {
        return m_info;
    }

private:
    bool samples_available() const;

    // callback implementations
    FLAC__StreamDecoderReadStatus   readCallbackImpl( FLAC__byte buffer[], size_t* bytes );
    FLAC__StreamDecoderSeekStatus   seekCallbackImpl( FLAC__uint64 absolute_byte_offset );
    FLAC__StreamDecoderTellStatus   tellCallbackImpl( FLAC__uint64* absolute_byte_offset );
    FLAC__StreamDecoderLengthStatus lengthCallbackImpl( FLAC__uint64* stream_length );
    FLAC__bool                      eofCallbackImpl();
    bool                            writeCallbackImpl( const FLAC__Frame* frame, const FLAC__int32* const buffer[] );
    void                            metadataCallbackImpl( const FLAC__StreamMetadata* metadata );
    void                            errorCallbackImpl( FLAC__StreamDecoderErrorStatus status );

    Source                 m_source;
    audio::ifstream_info   m_info;
    std::vector<char_type> m_buffer;                // buffer (in bytes) used by FLAC decoder
    size_t                 m_buffer_offset;         // buffer offset of next byte to return
    bool                   m_audio_block_available; // is there data in the write buffer?
    std::streampos         m_pos = 0;

    using DecoderPtr = std::unique_ptr<FLAC__StreamDecoder, std::function<void( FLAC__StreamDecoder* )>>;
    DecoderPtr m_decoder;
};


template <class Source>
flac_source<Source>::flac_source( Source&& source )
: m_source( std::move( source ) )
, m_buffer_offset( 0 )
, m_audio_block_available( false )
, m_decoder( DecoderPtr( ::FLAC__stream_decoder_new(), []( FLAC__StreamDecoder* ptr ) {
    if ( ptr )
    {
        FLAC__stream_decoder_finish( ptr );
        FLAC__stream_decoder_delete( ptr );
    }
} ) )
{
    if ( !m_decoder )
        throw std::runtime_error( "flac_file_source: Could not instantiate decoder." );


    auto read_callback = []( const FLAC__StreamDecoder*, FLAC__byte buffer[], size_t* bytes, void* client_data ) {
        return static_cast<flac_source*>( client_data )->readCallbackImpl( buffer, bytes );
    };

    auto seek_callback = []( const FLAC__StreamDecoder*, FLAC__uint64 absolute_byte_offset, void* client_data ) {
        return static_cast<flac_source*>( client_data )->seekCallbackImpl( absolute_byte_offset );
    };

    auto tell_callback = []( const FLAC__StreamDecoder*, FLAC__uint64* absolute_byte_offset, void* client_data ) {
        return static_cast<flac_source*>( client_data )->tellCallbackImpl( absolute_byte_offset );
    };

    auto length_callback = []( const FLAC__StreamDecoder*, FLAC__uint64* stream_length, void* client_data ) {
        return static_cast<flac_source*>( client_data )->lengthCallbackImpl( stream_length );
    };

    auto eof_callback = []( const FLAC__StreamDecoder*, void* client_data ) {
        return static_cast<flac_source*>( client_data )->eofCallbackImpl();
    };

    auto write_callback = []( const FLAC__StreamDecoder*,
                              const FLAC__Frame*       frame,
                              const FLAC__int32* const buffer[],
                              void*                    client_data ) {
        if ( !static_cast<flac_source*>( client_data )->writeCallbackImpl( frame, buffer ) )
        {
            return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
        }

        return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
    };

    auto metadata_callback = []( const FLAC__StreamDecoder*, const FLAC__StreamMetadata* metadata, void* client_data ) {
        static_cast<flac_source*>( client_data )->metadataCallbackImpl( metadata );
    };

    auto error_callback = []( const FLAC__StreamDecoder*, FLAC__StreamDecoderErrorStatus status, void* client_data ) {
        static_cast<flac_source*>( client_data )->errorCallbackImpl( status );
    };


    auto status = FLAC__stream_decoder_init_stream( m_decoder.get(),
                                                    read_callback,
                                                    seek_callback,
                                                    tell_callback,
                                                    length_callback,
                                                    eof_callback,
                                                    write_callback,
                                                    metadata_callback,
                                                    error_callback,
                                                    this );

    if ( status == FLAC__STREAM_DECODER_INIT_STATUS_ERROR_OPENING_FILE )
        throw std::runtime_error( "flac_file_source: Error opening file" );
    else if ( status != FLAC__STREAM_DECODER_INIT_STATUS_OK )
        throw std::runtime_error( "flac_file_source: Could not initialize FLAC decoder." );

    // read metadata
    FLAC__stream_decoder_set_metadata_respond( m_decoder.get(), FLAC__METADATA_TYPE_STREAMINFO );
    if ( !FLAC__stream_decoder_process_until_end_of_metadata( m_decoder.get() ) )
    {
        throw std::runtime_error( "FLAC: Can't decode metadata" );
    }
}

//----------------------------------------------------------------------------------------------------------------------

template <class Source>
bool flac_source<Source>::samples_available() const
{
    return m_audio_block_available
           || ( FLAC__stream_decoder_process_single( m_decoder.get() )
                && FLAC__STREAM_DECODER_END_OF_STREAM != FLAC__stream_decoder_get_state( m_decoder.get() ) );
}
//----------------------------------------------------------------------------------------------------------------------

template <typename Source>
FLAC__StreamDecoderReadStatus flac_source<Source>::readCallbackImpl( FLAC__byte buffer[], size_t* bytes )
{
    if ( *bytes > 0 )
    {
        auto ret = m_source.read( reinterpret_cast<char*>( buffer ), *bytes );
        if ( ret < 0 )
            return FLAC__STREAM_DECODER_READ_STATUS_ABORT;

        *bytes = static_cast<size_t>( ret );

        if ( *bytes == 0 )
            return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
        else
            return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
    }
    else
        return FLAC__STREAM_DECODER_READ_STATUS_ABORT; /* abort to avoid a deadlock */
}
//----------------------------------------------------------------------------------------------------------------------
template <typename Source>
FLAC__StreamDecoderSeekStatus flac_source<Source>::seekCallbackImpl( FLAC__uint64 absolute_byte_offset )
{
    if ( false /*TODO is Source::seekable*/ )
        return FLAC__STREAM_DECODER_SEEK_STATUS_UNSUPPORTED;
    else if ( m_source.seek( absolute_byte_offset, BOOST_IOS::beg ) < 0 )
        return FLAC__STREAM_DECODER_SEEK_STATUS_ERROR;
    else
        return FLAC__STREAM_DECODER_SEEK_STATUS_OK;
}
//----------------------------------------------------------------------------------------------------------------------
template <typename Source>
FLAC__StreamDecoderTellStatus flac_source<Source>::tellCallbackImpl( FLAC__uint64* absolute_byte_offset )
{
    auto pos = m_source.seek( 0, BOOST_IOS::cur );
    if ( pos < 0 )
        return FLAC__STREAM_DECODER_TELL_STATUS_ERROR;
    else
    {
        *absolute_byte_offset = static_cast<FLAC__uint64>( pos );
        return FLAC__STREAM_DECODER_TELL_STATUS_OK;
    }
}
//----------------------------------------------------------------------------------------------------------------------
template <typename Source>
FLAC__StreamDecoderLengthStatus flac_source<Source>::lengthCallbackImpl( FLAC__uint64* stream_length )
{
    auto pos = m_source.seek( 0, BOOST_IOS::end );
    if ( pos < 0 )
        return FLAC__STREAM_DECODER_LENGTH_STATUS_ERROR;
    else
    {
        *stream_length = static_cast<FLAC__uint64>( pos );
        return FLAC__STREAM_DECODER_LENGTH_STATUS_OK;
    }
}
//----------------------------------------------------------------------------------------------------------------------
template <typename Source>
FLAC__bool flac_source<Source>::eofCallbackImpl()
{
    return m_source.seek( 0, BOOST_IOS::cur ) < 0;
}

//----------------------------------------------------------------------------------------------------------------------
// process audio frames and copy from internal buffer to client buffer

template <class Source>
std::streamsize flac_source<Source>::read( char_type* s, std::streamsize n )
{
    assert( 0 == n % m_info.bytes_per_frame() );

    size_t requested = size_t( n ); // total bytes requested
    size_t delivered = 0;           // accumulator for bytes read

    while ( delivered < requested && samples_available() )
    {
        // copy at most (requested - delivered) bytes to s
        size_t to_copy = std::min( m_buffer.size() - m_buffer_offset, requested - delivered );
        s              = std::copy_n( m_buffer.begin() + m_buffer_offset, to_copy, s );
        delivered += to_copy;
        m_buffer_offset += to_copy;

        if ( m_buffer_offset == m_buffer.size() )
        {
            m_audio_block_available = false;
            m_buffer_offset         = 0;
        }
    }

    m_pos += delivered;
    return delivered;
}

//----------------------------------------------------------------------------------------------------------------------

template <class Source>
std::streampos flac_source<Source>::seek( boost::iostreams::stream_offset off, BOOST_IOS::seekdir way )
{
    assert( 0 == off % m_info.bytes_per_frame() );

    const auto beg = std::streampos( 0 );
    const auto end = std::streampos( info().num_bytes() );
    const auto pos = absolute_position( m_pos, beg, end, off, way );

    const auto frame_pos   = FLAC__uint64( pos / m_info.bytes_per_frame() );
    const auto byte_offset = FLAC__uint64( pos % m_info.bytes_per_frame() );

    if ( FLAC__stream_decoder_seek_absolute( m_decoder.get(), frame_pos ) )
    {
        m_pos           = pos;
        m_buffer_offset = size_t( byte_offset );
    }
    return m_pos;
}

//----------------------------------------------------------------------------------------------------------------------
// FLAC callback - "write" (copy) the FLAC frame of samples to our internal buffer

template <class Source>
bool flac_source<Source>::writeCallbackImpl( const FLAC__Frame* frame, const FLAC__int32* const buffer[] )
{

    auto channels     = m_info.num_channels();
    auto sample_bytes = m_info.bytes_per_sample();
    auto total_bytes  = frame->header.blocksize * sample_bytes * channels;

    if ( m_buffer.size() != total_bytes )
        m_buffer.resize( total_bytes );

    // copy from FLAC buffer (samples) to internal buffer (bytes)
    for ( size_t u = 0; u < frame->header.blocksize; ++u )
    {
        for ( size_t channel = 0; channel < channels; ++channel )
        {

            // Interleave samples
            auto sample       = buffer[channel][u];
            auto sample_index = sample_bytes * ( u * channels + channel );

            // Copy sample bytes into writeBuffer
            for ( size_t b = 0; b < sample_bytes; ++b )
            {
                char_type c                = ( sample >> ( 8 * b ) ) & 0xFF;
                m_buffer[sample_index + b] = c;
            }
        }
    }

    m_audio_block_available = true;
    return true;
}

//----------------------------------------------------------------------------------------------------------------------
// FLAC callback to write metadata into audio::stream_info

template <class Source>
void flac_source<Source>::metadataCallbackImpl( const FLAC__StreamMetadata* metadata )
{
    if ( metadata->type != FLAC__METADATA_TYPE_STREAMINFO )
        return;
    auto meta_info = metadata->data.stream_info;

    if ( meta_info.total_samples == 0 )
        throw std::runtime_error( "Empty File" );

    // FLAC supports from 1 to 8 channels per stream.
    if ( meta_info.channels < 1u || meta_info.channels > 8u )
        throw std::runtime_error( "Invalid number of channels: " + std::to_string( meta_info.channels ) );

    // FLAC supports linear PCM samples with a resolution between 4 and 32 bits per sample.
    if ( meta_info.bits_per_sample < 4u || meta_info.bits_per_sample > 32u )
        throw std::runtime_error( "Invalid bits per sample: " + std::to_string( meta_info.bits_per_sample ) );

    // FLAC supports linear sample rates from 1Hz - 655350Hz in 1Hz increments.
    if ( meta_info.sample_rate < 1u || meta_info.sample_rate > 655350u )
        throw std::runtime_error( "Invalid sample rate: " + std::to_string( meta_info.sample_rate ) );

    // We only support constant blocksize at this point.
    if ( meta_info.min_blocksize != meta_info.max_blocksize )
        throw std::runtime_error( "Only Constant blocksize supported " );

    // fill and set audio::stream_info
    audio::ifstream_info info;
    info.lossless( true );
    info.codec( audio::ifstream_info::codec_type::flac );
    info.container( audio::ifstream_info::container_type::flac );
    info.num_channels( meta_info.channels );
    info.num_frames( static_cast<size_t>( meta_info.total_samples ) );
    info.sample_rate( meta_info.sample_rate );
    info.format( pcm::format( pcm::signed_integer, meta_info.bits_per_sample, pcm::little_endian ) );
    {
        // 'pos' holds the position after reading the meta info (position before the audio data). 
        const auto pos = m_source.seek( 0, BOOST_IOS::cur );
        // Since we are interested in the audio bytes we subtract 'pos' from the file size.
        const auto audio_bytes = static_cast<size_t>( m_source.seek( 0, BOOST_IOS::end ) - pos );
        m_source.seek( pos, BOOST_IOS::beg );
        info.bit_rate( audio_bytes * 8 * info.sample_rate() / info.num_frames() );
    }
    m_info = std::move( info );
}

//----------------------------------------------------------------------------------------------------------------------

template <class Source>
void flac_source<Source>::errorCallbackImpl( FLAC__StreamDecoderErrorStatus /*status*/ )
{
    // TODO: pass error message to ifstream. Do not throw a runtime error as this callback gets called during decoding.
    // auto message = std::string( "Error decoding FLAC file: " ) + FLAC__StreamDecoderErrorStatusString[status] );
}
