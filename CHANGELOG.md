# 2024-05-08 version v1.1.2:

  * FIXED: Issues in cmake integration for NI's internal build system

# 2024-04-30 version v1.1.1:

  * ADDED: Cmake integration for NI's internal build system
  * FIXED: An issue where `media_foundation_helper` could produce an integer overflow resulting in a freeze while reading a file

# 2023-12-20 version v1.1.0:

  * ADDED: Possibility of creating a custom decoder via `istream_source`
  * ADDED: Possibility of creating a custom byte-stream backend of existing decoders
  * ADDED: Public access to bit rate for decoders
  * ADDED: Cmake support for unity builds in the `audiostream` library
  * CHANGED: Use Github Actions instead of Azure CI
  * FIXED: An issue where `media_foundation_source` could produce audio crackling on Windows 11
  * FIXED: An issue where seeking chunks in `wav_source` could result in an infinite loop for long files

# 2020-12-02 version v1.0.0:

  * Initial version tag
