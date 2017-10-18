# audio::istream

The base class for all audio input streams.

## unformatted input operations
Inherited from [`std::istream`](http://en.cppreference.com/w/cpp/io/basic_istream).
- [`read()`](http://en.cppreference.com/w/cpp/io/basic_istream/read): extracts
  blocks of characters 
- [`gcount()`](http://en.cppreference.com/w/cpp/io/basic_istream/gcount): returns
  number of characters extracted by last unformatted input operation

⚠️ Using these operations should be avoided. They are provided for maximum
flexibility if clients need to operate on the raw pcm data. In most use cases,
[formatted input operations](#formatted-input-operations) can and should be used
as they provide type safety and are optimized for best performance.

## positioning operations
Inherited from [`std::istream`](http://en.cppreference.com/w/cpp/io/basic_istream).
- [`tellg()`](http://en.cppreference.com/w/cpp/io/basic_istream/tellg): returns the
  input position indicator 
- [`seekg()`](http://en.cppreference.com/w/cpp/io/basic_istream/seekg): sets the input
  position indicator

⚠️ Using these operations should be avoided. They are provided for maximum
flexibility if clients need to operate on the raw pcm data. It is the clients
reponsibility to ensure that the position is aligned withing a sample or frame.
Therefore it is recommended to use [sample based positioning operations
(#sample-based-positioning-operations) instead.

## state functions
Inherited from [`std::basic_ios`](http://en.cppreference.com/w/cpp/io/basic_ios).
- [`good()`](http://en.cppreference.com/w/cpp/io/basic_ios/good): checks if no error
  has occurred i.e. I/O operations are available 
- [`eof()`](http://en.cppreference.com/w/cpp/io/basic_ios/eof): checks if end-of-file
  has been reached 
- [`fail()`](http://en.cppreference.com/w/cpp/io/basic_ios/fail): checks if an error
  has occurred 
- [`bad()`](http://en.cppreference.com/w/cpp/io/basic_ios/bad): checks if a
  non-recoverable error has occurred 
- [`operator!()`](http://en.cppreference.com/w/cpp/io/basic_ios/operator!): checks if an
  error has occurred (synonym of `fail()`)
- [`operator bool()`](http://en.cppreference.com/w/cpp/io/basic_ios/operator%20bool): checks
  if no error has occurred (synonym of `!fail()`) 
- [`rdstate()`](http://en.cppreference.com/w/cpp/io/basic_ios/rdstate): returns state flags 
- [`setstate()`](http://en.cppreference.com/w/cpp/io/basic_ios/setstate): sets state flags 
- [`clear()`](http://en.cppreference.com/w/cpp/io/basic_ios/clear): modifies state flags

## miscellaneous
- [`rdbuf`](http://en.cppreference.com/w/cpp/io/basic_ios/rdbuf): Returns the associated
  stream buffer. If there is no associated stream buffer, returns a null pointer.

Accessing the associated stream buffer might be usefull for i.e. wrapping an audio::stream
into an std::istream (and interface with other std functions) :

 ```cpp
std::string to_string(audio::istream& stream)
{
    std::istream ref_stream( stream.rdbuf() ); // a reference to the stream

    return { std::istreambuf_iterator<char>(ref_stream), std::istreambuf_iterator<char>() };

}
 ```


## formatted input operations

These operations extracts one or more samples from the stream, by converting the underlying
pcm data sequence into a sequence of samples. The sample format can be any floating point
type ( i.e. `float`, `double`) or integral type (e.g `int8_t`, `int16_t`, `int32_t`,
`int64_t`, `uint8_t`, `uint16_t`, `uint32_t`, `uint64_t`)

- [`operator>>`](): extracts samples from the stream.
    + The value-based operator extracts a single sample from the stream, in analogy to
    [`std::istream`](http://en.cppreference.com/w/cpp/io/basic_istream)`::`
    [`operator>>`](http://en.cppreference.com/w/cpp/io/basic_istream/operator_gtgt2).
    + The range-based operator extracts a range of samples from the stream. If the stream
    reaches the end of file ([`eof`](http://en.cppreference.com/w/cpp/io/basic_ios/eof))
    during this operation, the remaining samples in the range will be set to the sample
    format's neutral element (which is 0 in most cases). This comes in handy as audio
    processing is mostly block-based and the last block usually needs to be zero-padded anyways.

⚠️ It is highly recommended to use the range-based version instead of the value-based version
as this will result in a better performance (during pcm conversion). 

I.e. clients should stream block wise:

 ```cpp
void process(audio::istream& stream)
{
    std::vector<float> buffer(256);
    while (stream >> buffer)
    {
        // process whole buffer
    }
}
 ```

rather than sample wise:

 ```cpp
void process(audio::istream& stream)
{
    float value;
    while (stream >> value)
    {
        // process sample
    }
}
 ```


## sample based positioning operations:

- `sample_tellg()`: returns the input position indicator of the current sample
- `frame_tellg()`: returns the input position indicator of the current frame
- `sample_seekg()`: sets the input position indicator to the sample position
- `frame_seekg()`: sets the input position indicator to the frame position
 

## audio specific operations

- `info()`: returns the stream info [`audio::istream_info`](../inc/ni/media/audio/istream_info.h)
  with contains audio specific information like:
     + `num_channels`: the number of channels
     + `num_frames`: the number of sample frames ( one frame consist of num_channels samples )
     + `sample_rate`: the sample rate 
     + `format`: the pcm format the audio data is stored in
     + …
 
⚠️ The info may contain erroneous information in case the stream is corrupted. Clients have to
check if the info is plausible in their own context (i.e. `num_channels <= 2` when expecting
stereo data number of frames <= 10 minutes of audio, etc.).
