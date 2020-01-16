#include <ni/media/audio/flac/flac_file_sink.h>
#include <ni/media/iostreams/positioning.h>

#include <functional>

#define FLAC__NO_DLL
#include <FLAC++/encoder.h>

#include <iostream>

//----------------------------------------------------------------------------------------------------------------------

class flac_file_sink::Impl
{
    using EncoderPtr = std::unique_ptr<FLAC__StreamEncoder, std::function<void( FLAC__StreamEncoder* )>>;

public:
    Impl( const std::string& path, const info_type& info );

    auto write( const char_type* s, std::streamsize n ) -> std::streamsize;

    auto info() const -> audio::ofstream_info;

private:
    EncoderPtr m_encoder;

    audio::ofstream_info m_info;
    std::streamsize      m_pos;
};

flac_file_sink::Impl::Impl( const std::string& path, const info_type& info )
: m_encoder( EncoderPtr( ::FLAC__stream_encoder_new(),
                         []( FLAC__StreamEncoder* ptr ) {
                             if ( ptr )
                             {
                                 FLAC__stream_encoder_finish( ptr );
                                 FLAC__stream_encoder_delete( ptr );
                             }
                         } ) )
, m_info( info )
, m_pos( 0 )
{
    if ( !m_encoder )
        throw std::runtime_error( "flac_file_sink: Could not instantiate encoder." );

    FLAC__stream_encoder_set_channels( m_encoder.get(), m_info.num_channels() );
    FLAC__stream_encoder_set_bits_per_sample( m_encoder.get(), m_info.bits_per_sample() );
    FLAC__stream_encoder_set_sample_rate( m_encoder.get(), m_info.sample_rate() );

    auto status = FLAC__stream_encoder_init_file( m_encoder.get(), path.c_str(), nullptr, this );

    if ( status != 0 )
        throw std::runtime_error( "flac_file_sink: Could not initialize encoder. Error status: "
                                  + std::to_string( status ) );
}

//----------------------------------------------------------------------------------------------------------------------

auto flac_file_sink::Impl::info() const -> audio::ofstream_info
{
    return m_info;
}

//----------------------------------------------------------------------------------------------------------------------

auto flac_file_sink::Impl::write( const char_type* s, std::streamsize n ) -> std::streamsize
{
    size_t      frames = static_cast<size_t>( n );
    FLAC__int32 buffer[frames];

    std::copy( s, s + frames, buffer );

    if ( !FLAC__stream_encoder_process_interleaved( m_encoder.get(), buffer, frames / info().num_channels() ) )
    {
        throw std::runtime_error( "flac_file_sink: Error writing data to file." );
    }
    m_pos += n;
    return n;
}

//----------------------------------------------------------------------------------------------------------------------

flac_file_sink::flac_file_sink() = default;

//----------------------------------------------------------------------------------------------------------------------

flac_file_sink::~flac_file_sink() = default;

//----------------------------------------------------------------------------------------------------------------------

flac_file_sink::flac_file_sink( flac_file_sink&& ) = default;

//----------------------------------------------------------------------------------------------------------------------

void flac_file_sink::open( const std::string& path )
{
    m_impl.reset( new Impl( path, m_info ) );
}

//----------------------------------------------------------------------------------------------------------------------

flac_file_sink::flac_file_sink( const info_type& info, const std::string& path )
: m_info( info )
{
    open( path );
}

//----------------------------------------------------------------------------------------------------------------------

auto flac_file_sink::write( const char_type* s, std::streamsize n ) -> std::streamsize
{
    return m_impl->write( s, n );
}

//----------------------------------------------------------------------------------------------------------------------

void flac_file_sink::close()
{
    m_impl.release();
}

//----------------------------------------------------------------------------------------------------------------------

auto flac_file_sink::info() const -> info_type
{
    return m_info;
}


//----------------------------------------------------------------------------------------------------------------------
