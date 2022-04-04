# NI MEDIA [![Build Status](https://dev.azure.com/mohinda/ni-media/_apis/build/status/marcrambo.ni-media?branchName=master)](https://dev.azure.com/mohinda/ni-media/_build/latest?definitionId=1&branchName=master) [![codecov](https://codecov.io/gh/NativeInstruments/ni-media/branch/master/graph/badge.svg)](https://codecov.io/gh/NativeInstruments/ni-media)

NI Media is a library for reading from / writing to audio streams developed at [Native Instruments](https://www.native-instruments.com).

## Motivation

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

## Components

 - [audiostream](audiostream/README.md):  the main library for reading from / writing to audio streams.
 - [pcm](pcm/README.md): a small library to convert [pcm](https://en.wikipedia.org/wiki/Pulse-code_modulation)
   data from / to [arithmetic](http://en.cppreference.com/w/cpp/language/types) types.


## Dependencies

* [boost](https://github.com/boostorg/boost) ( algorithm, endian, filesystem, format, icl, iostream, local, program-option, regex, system)
* [flac](https://github.com/xiph/flac) & [ogg](https://github.com/xiph/ogg), for  flac support (CMake option `NIMEDIA_ENABLE_FLAC_DECODING`)
* [vorbis](https://github.com/xiph/vorbis) & [ogg](https://github.com/xiph/ogg), for ogg vorbis support (CMake option  `NIMEDIA_ENABLE_OGG_DECODING`)
* [googletest](https://github.com/google/googletest) for building the tests (CMake option `NIMEDIA_TESTS`)

## Platforms

ni-media requires a c++14 compliant compiler and is currently supported and tested on these platforms: 

* Windows ( requires Visual Studio 2015 Update 3  or higher )
* Mac OS X ( requires Xcode / Clang 7.3 or higher )
* iOS ( requires Xcode / Clang 7.3 or higher )
* Linux ( requires GCC 5.0 or higher, Clang 7.3 or higher should also work )


## Building

First, build and install boost `filesystem`, `iostream`, `system` and `program-option` to `path/to/dependencies`. Optionally install any codecs that you want to use, for example [flac](https://github.com/xiph/flac) and [ogg-vorbis](https://github.com/xiph/vorbis).
Now configure ni-media with [CMake](https://cmake.org/) (version 3.16.0 or higher is required)

```
cmake -G YOUR-PROJECT-GENERATOR -DCMAKE_PREFIX_PATH=path/to/dependencies/ path/to/ni-media
```

Specific codecs can be enabled / disabled by passing additional CMake [options](audiostream/doc/ifstream.md).

We can now build ni-media:
```
cmake --build . 
```

## Testing

[googletest](https://github.com/google/googletest) is required for testing and needs to be installed to `path/to/dependencies`. The unit tests can be enabled with CMake option `NIMEDIA_TESTS`.

```
cmake -G YOUR-PROJECT-GENERATOR -DCMAKE_PREFIX_PATH=path/to/dependencies/ -DNIMEDIA_TESTS=ON path/to/ni-media
```

Before running the tests some reference test files need to be downloaded using [git-lfs](https://github.com/git-lfs/git-lfs):
```
git lfs pull -X ""
```

To execute the tests run:
```
cmake --build . --target test
```

## Contributions

We very much appreciate your contribution! If you want to contribute please get in touch with the maintainers:

* [Marc Boucek](https://github.com/marcrambo) 
* [Nathan Kozlowski](https://github.com/ni-nkozlowski)
* [Moritz Heppner](https://github.com/ni-mheppner)

Please run [clang-format](https://clang.llvm.org/docs/ClangFormat.html) with the provided .clang-format file and if possible add some tests when opening a pull request.

