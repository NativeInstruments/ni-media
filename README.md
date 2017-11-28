# NI MEDIA [![Travis Build Status](https://travis-ci.org/NativeInstruments/ni-media.svg?branch=master)](https://travis-ci.org/NativeInstruments/ni-media) [![Appveyor Build Status](https://ci.appveyor.com/api/projects/status/github/marcrambo/ni-media?branch=master&svg=true)](https://ci.appveyor.com/project/marcrambo/ni-media/history) 

NI Media is a library for reading from / writing to audio streams developed at [Native Instruments](https://www.native-instruments.com).

The goal is to have a modern C++ library for dealing with audio streams in an idiomatic C++ style.

Modern:
- a clear separation of concerns (modular instead of fat classes)
- support for ranges and iterators

Idiomatic:
- based on std.streams.
- integrates well with STL algorithms and boost


The following example demonstrates how to stream an entire audio file into a vector:

```cpp
#include <ni/media/audio/ifstream.h>
#include <vector>

int main()
{
    auto stream = audio::ifstream("hello.wav");
    auto samples = std::vector<float>(stream.info().num_samples());
    stream >> samples;
    // use samples
}
```


Currently the library consists of the following components:

 - [audiostream](audiostream/README.md):  the main library for reading from / writing to audio streams.
 - [pcm](pcm/README.md): a small library to convert [pcm](https://en.wikipedia.org/wiki/Pulse-code_modulation)
   data from / to [arithmetic](http://en.cppreference.com/w/cpp/language/types) types.