# `audio::ifstream`

The base class for all audio file input streaming. Inherits from [`audio::istream`](istream.md).

Similar to `std::ifstrem`, its contructor takes a filepath as argument. Here the filepath must
point to an audio file currently supported by ni-media. At this date supported containers are:
 - **wav**: native support, enabled through CMake Option `NIMEDIA_ENABLE_AIFF_DECODING`
 - **aiff**: native support, enabled through CMake Option `NIMEDIA_ENABLE_WAV_DECODING`
 - **flac**: requires libFlac & LibOgg, enabled through CMake Option `NIMEDIA_ENABLE_FLAC_DECODING`
 - **ogg**: requires libVorbis & LibOgg, enabled through CMake Option `NIMEDIA_ENABLE_OGG_DECODING`
 - **mp3**: not supported on linux, enabled through CMake Option `NIMEDIA_ENABLE_MP3_DECODING`
 - **mp4**: not supported on linux, enabled through CMake Option `NIMEDIA_ENABLE_MP4_DECODING`

The constructor will throw a runtime error if the file format is not supported or a codec error
occured (e.g. corrupted file).

Example code:

 ```cpp
void analyze_file( const std::string& filename )
{
    audio::ifstream stream;
    try
    {
        stream = audio::ifstream( filename );
    }
    catch ( ... )
    {
        // handle exception

        return;
    }

    // stream from file...
}
 ```

You can also allow only a specific container:

 ```cpp
void analyze_wav_file( const std::string& filename )
{
    audio::ifstream stream;
    try
    {
        stream = audio::ifstream( filename, audio::ifstream::info_type::container_type::wav );
    }
    catch ( ... )
    {
        // handle exception

        return;
    }

    // stream from file...
}
 ```

An `audio::ifstream` also supports multi stream containers, which allows us to stream from e.g. a
Native Instruments [Stems](https://www.native-instruments.com/en/specials/stems/) file.
A stem file consists of five audio streams and additional metadata packed inside an mp4 container.
By default, an `audio::ifstream` will open the Stems Master Stream, but is also capable of opening
any of the other streams by simply passing the desired stream index as argument to the constructor:

 ```cpp
void analyze_stem_stream( const std::string& filename, size_t stream_index )
{
    audio::ifstream stream;
    try
    {
        stream = audio::ifstream( filename, audio::ifstream::info_type::container_type::mp4, stream_index );
    }
    catch ( ... )
    {
        // handle exception

        return;
    }

    // stream from file...
}
 ```
