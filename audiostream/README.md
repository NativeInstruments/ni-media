# audiostream

This library provides audio streams and is based on the standard [stdio](http://en.cppreference.com/w/cpp/io)
library.

In fact an audio stream can be modelled as an std stream which: 
 - contains multichanel interleaved [pcm](https://en.wikipedia.org/wiki/Pulse-code_modulation) data
 - provides additional audio specific information required to playback the stream

## Stream-based I/O

The audiostream library is organized around abstract input/output audio devices. These abstract devices
allow the same code to handle input/output to files, memory streams, or custom adaptor devices that perform
arbitrary operations (e.g. codec specific audio decoding) on the fly.

## Abstraction

 - [audio::istream](doc/istream.md):  wraps a given abstract audio device
   and provides high-level output interface 
 - [audio::ostream](doc/ostream.md):  wraps a given abstract audio device and provides high-level
   input interface 

## File I/O implementation

 - [audio::ifstream](doc/ifstream.md):  implements high-level audio file stream input operations 
 - [audio::ofstream](doc/ofstream.md):  implements high-level audio file stream output operations
