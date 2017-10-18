# audio::ostream

The base class for all audio output streams.


## unformatted output operations

Inherited from [`std::ostream`](http://en.cppreference.com/w/cpp/io/basic_ostream)

- [write()](http://en.cppreference.com/w/cpp/io/basic_ostream/write): inserts blocks of characters 

⚠️ Using these operations should be avoided. They are provided for maximum flexibility if clients
need to operate on the raw pcm data. In most use cases,
[formatted output operations](#formatted-output-operations) can and should be used as they provide
type safety and are optimized for best performance.


## positioning operations

Inherited from [std::ostream](http://en.cppreference.com/w/cpp/io/basic_ostream).

- [`tellp()`](http://en.cppreference.com/w/cpp/io/basic_ostream/tellp): returns the output
  position indicator 
- [`seekp()`](http://en.cppreference.com/w/cpp/io/basic_ostream/seekp): ⚠️ is not implemented
  yet. It is unclear at this point if this fucntionality can be / will be added later on, as it
  seems not all codecs support seeking while encoding.

⚠️ Using these operations should be avoided. They are provided for maximum flexibility if
clients need to operate on the raw pcm data. It is the clients reponsibility to ensure that
the position is aligned withing a sample or frame. Therefore it is recommended to use
[sample based positioning operations](#sample-based-positioning-operations) instead. 


## state functions

Inherited from [std::basic_ios](http://en.cppreference.com/w/cpp/io/basic_ios).

- [`good()`](http://en.cppreference.com/w/cpp/io/basic_ios/good): checks if no error has occurred
  i.e. I/O operations are available 
- [`eof()`](http://en.cppreference.com/w/cpp/io/basic_ios/eof): checks if end-of-file has been reached
- [`fail()`](http://en.cppreference.com/w/cpp/io/basic_ios/fail): checks if an error has occurred 
- [`bad()`](http://en.cppreference.com/w/cpp/io/basic_ios/bad): checks if a non-recoverable error
  has occurred 
- [`operator!()`](http://en.cppreference.com/w/cpp/io/basic_ios/operator!): checks if an error has
  occurred (synonym of fail())
- [`operator bool()`](http://en.cppreference.com/w/cpp/io/basic_ios/operator%20bool): checks if no error
  has occurred (synonym of !fail()) 
- [`rdstate()`](http://en.cppreference.com/w/cpp/io/basic_ios/rdstate): returns state flags
- [`setstate()`](http://en.cppreference.com/w/cpp/io/basic_ios/setstate): sets state flags
- [`clear()`](http://en.cppreference.com/w/cpp/io/basic_ios/clear): modifies state flags


## formatted output operations

These operations inserts one or more samples into the stream, by converting the sequence of samples
to a pcm data sequence which is then insterted into the stream.  The sample format can be any floating
point type (i.e. `float`, `double`) or integral type (e.g `int8_t`, `int16_t`, `int32_t`, `int64_t`,
`uint8_t`, `uint16_t`, `uint32_t`, `uint64_t`).

- [`operator<<`](): inserts samples into the stream.
    + The value-based operator inserts a single sample into the stream, in analogy to
      [`std::ostream`](http://en.cppreference.com/w/cpp/io/basic_ostream)
      [`operator<<`](http://en.cppreference.com/w/cpp/io/basic_ostream/operator_ltlt2).
    + The range-based operator inserts a range of samples into the stream.

 ⚠️ It is highly recommended to use the range-based version instead of the value-based version, as
 this will result in a better performance (during pcm conversion). 

I.e. clients should stream block wise:

 ```cpp
void fill(audio::ostream& stream, const std::vector<float>& samples)
{
    stream << samples;
}
 ```

rather than sample wise:

 ```cpp
void fill(audio::istream& stream, const std::vector<float>& samples)
{
    for (auto sample : samples)
    {
        stream << sample;
    }
}
 ```


## sample based positioning operations

- `sample_tellp()`: returns the output position indicator of the current sample.
- `frame_tellp()`: returns the output position indicator of the current frame.


## audio specific operations

- `info()`: returns the stream info [`audio::ostream_info`](../inc/ni/media/audio/ostream_info.h)
  which contains audio specific information like:
     + `num_channels`: the number of channels
     + `num_frames`: the number of sample frames (one frame consist of `num_channels` samples)
     + `sample_rate`: the sample rate
     + `format`: the pcm format the audio data is stored in
     + …
 