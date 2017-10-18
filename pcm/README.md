# pcm

The pcm library consists of tools to convert [pcm](https://en.wikipedia.org/wiki/Pulse-code_modulation)
data from / to [arithmetic](http://en.cppreference.com/w/cpp/language/types) types.


## `pcm::format`

The `pcm::format` consists of 3 properties:

- `number` (the underlying representation): 
    + `signed_integer`: signed integer types (`int8_t`, `int16_t`, …)
    + `unsigned_integer`: unsigned integer types (`uint8_t`, `uint16_t`, …)
    + `floating_point`: floating point types (`float`, `double`)
- `bitwidth` (the number of bits used to represent one pcm sample)
    + `_8bit`: 8 bits per sample
    + `_16bit`: 16 bits per sample
    + `_24bit`: 24 bits per sample
    + `_32bit`: 32 bits per sample
    + `_64bit`: 64 bits per sample
- `endian` (the byte endianness): 
    + `big_endian`: most significant byte (MSB) comes first in memory
    + `little_endian`: least significant byte (LSB) comes first in memory
    + `native_endian`: the natural endianness of the current platform (either
      `big_endian` or `little_endian`)


A format can be defined at runtime or at compiletime (see [pcm::iterator](#pcmiterator)
to underatand why this is usefull). 

A compiletime format can be created by passing format properties as template parameters
to `pcm::make_format()`:
```cpp
auto compiletime_format = pcm::make_format<pcm::signed_integer,pcm::_8bit, pcm::little_endian>(); 
```

A runtime format can be created by passing format properties as arguments to `pcm::make_format()`:
```cpp
auto runtime_format = pcm::make_format(pcm::signed_integer, pcm::_8bit, pcm::little_endian); 
```

## `pcm::iterator`

The `pcm::iterator` allows to iterate and convert from / to pcm data in a generic, type safe, and
performant manner. It wraps a char itererator interfacing the raw pcm data. The `pcm::iterator`'s
`value_type` can freely be chosen to any
[arithmetic type](http://en.cppreference.com/w/cpp/language/types), and a client will interact
with it as if it was iterating through a sequence of that value_type.

The best way to create a `pcm::iterator` is by using the helper function `pcm::make_iterator()`:
```cpp
template <typename Value, typename Iterator, typename Format>
auto make_iterator( Iterator it, Format f );
```

The three template parameters are:
 - Value: the `value_type` of the `pcm::iterator`
 - Iterator: the wrapped iterator (`value_type==char`)
 - Format: the [`pcm::format`](#pcmformat). This can be a runtime or compiletime format.

Depending on the Format, `pcm::make_iterator()` will return either a runtime or compiletime
`pcm::iterator`.

This will create a compiletime `pcm::iterator`:
```cpp
std::vector<char> pcm_data(256);

auto compiletime_format = pcm::make_format<pcm::signed_integer,pcm::_8bit, pcm::little_endian>(); 

auto compiletime_iterator = pcm::make_iterator<float>(pcm_data.begin(), compiletime_format);
```

This will create a runtime `pcm::iterator`:
```cpp
std::vector<char> pcm_data(256);

auto runtime_format = pcm::make_format(pcm::signed_integer,pcm::_8bit, pcm::little_endian); 

auto runtime_iterator = pcm::make_iterator<float>(pcm_data.begin(), runtime_format);
```

From a client's perspective both runtime and compiletime iterators behave identically. The only
difference is that the runtime version will be less performant than the compiletime version, due
to function indirections caused by the required type erasure).

So, why even provide a runtime version if it is less performant than the compiletime version? 

Often the `pcm::format` is not known at compiletime. I.e. when opening an audio file using an
[`audio::ifstream`](../audiostream/ifstream.md) the format is only known once the file has been opened.

In order to still achieve best possible performance, the pcm library provides some algorithms that
will be (almost) as performant when used with the runtime iterator as when used with the compiletime
iterator.


## pcm::algorithms

The following algorithms can and should be used with the `pcm::iterator` in order to achieve best
performance:
 - [`pcm::copy()`](inc/ni/media/pcm/algorithm/copy.h)
 - [`pcm::copy_n()`](inc/ni/media/pcm/algorithm/copy_n.h)










