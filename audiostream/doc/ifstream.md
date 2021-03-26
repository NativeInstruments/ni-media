# `audio::ifstream`

The base class for all audio file input streaming. Inherits from [`audio::istream`](istream.md).

Similar to `std::ifstrem`, its contructor takes a filepath as argument. Here the filepath must point to an audio file currently supported by ni-media. At this date supported containers are:
 - **wav**: native support, enabled through CMake Option `NIMEDIA_ENABLE_WAV_DECODING`
 - **aiff**: native support, enabled through CMake Option `NIMEDIA_ENABLE_AIFF_DECODING`
 - **flac**: requires libFlac & LibOgg, enabled through CMake Option `NIMEDIA_ENABLE_FLAC_DECODING`
 - **ogg**: requires libVorbis & LibOgg, enabled through CMake Option `NIMEDIA_ENABLE_OGG_DECODING`
 - **mp3**: not supported on linux, enabled through CMake Option `NIMEDIA_ENABLE_MP3_DECODING`
 - **mp4**: not supported on linux, enabled through CMake Option `NIMEDIA_ENABLE_MP4_DECODING`

The constructor will throw a runtime error if the file format is not supported or a codec error occured (e.g. corrupted file).

Example code:

```cpp
void analyze_file( const std::filesystem::path& filename )
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
void analyze_wav_file( const std::filesystem::path& filename )
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

An `audio::ifstream` also supports multi stream containers, which allows us to stream from e.g. a Native Instruments [Stems](https://www.native-instruments.com/en/specials/stems/) file. A stem file consists of five audio streams and additional metadata packed inside an mp4 container.
By default, an `audio::ifstream` will open the Stems Master Stream, but is also capable of opening any of the other streams by simply passing the desired stream index as argument to the constructor:

```cpp
void analyze_stem_stream( const std::filesystem::path& filename, size_t stream_index )
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

## audio specific operations

- `info()`: returns the stream info [`audio::ifstream_info`](../inc/ni/media/audio/ifstream_info.h)
  Extends `audio::istream_info` with audio file specific informations like:
     + `num_frames`: the number of sample frames ( one frame consist of **num_channels** samples )
     + `container`: the underlying container( **wav**, **aiff**, **flac**, **ogg**, **mp3**, **mp4** )
     + `codec` : the codec used: (  **wav**, **aiff**, **flac**, **vorbis**, **mp3**, **aac**, **alac** )

⚠️ `num_frames` may differ from the actual number of frames in the stream as this information relies on the codec. 
The only way to obtain the **exact** number of frames is by seeking to the end of stream and retrieving the frame position. 
In general the `num_frames` info provided by the codec is correct for **wav**, **aiff**, **flac**, **ogg**, **mp4** containers ( assuming no errors occured during encoding ). For **mp3** `num_frames` might be off by up to a few thousand frames. 
