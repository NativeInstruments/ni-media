#-----------------------------------------------------------------------------------------------------------------------
# build options
#-----------------------------------------------------------------------------------------------------------------------

option( NIMEDIA_ENABLE_AIFF_DECODING "Enable ni-media aiff decoding" ON )
option( NIMEDIA_ENABLE_AIFF_ENCODING "Enable ni-media aiff encoding" ON )
option( NIMEDIA_ENABLE_FLAC_DECODING "Enable ni-media flac decoding" ON )
option( NIMEDIA_ENABLE_OGG_DECODING  "Enable ni-media ogg decoding" ON )

if ( APPLE OR WIN32 )
  option( NIMEDIA_ENABLE_MP3_DECODING  "Enable ni-media mp3 decoding" ON )
  option( NIMEDIA_ENABLE_MP4_DECODING  "Enable ni-media mp4 decoding" ON )
else()
  option( NIMEDIA_ENABLE_MP3_DECODING  "Enable ni-media mp3 decoding" OFF )
  option( NIMEDIA_ENABLE_MP4_DECODING  "Enable ni-media mp4 decoding" OFF )
endif()

if ( IOS )
  option( NIMEDIA_ENABLE_ITUNES_DECODING "Enable ni-media iTunes decoding" ON )
else()
  option( NIMEDIA_ENABLE_ITUNES_DECODING "Enable ni-media iTunes decoding" OFF )
endif()

option( NIMEDIA_ENABLE_WAV_DECODING  "Enable ni-media wav decoding" ON )
option( NIMEDIA_ENABLE_WAV_ENCODING  "Enable ni-media wav encoding" ON )

# Wma decoding is only supported on windows and is currently untested. Use at your own risk!
option( NIMEDIA_ENABLE_WMA_DECODING "Enable ni-media wma decoding" OFF )

if( NOT NIMEDIA_ENABLE_AIFF_DECODING )
  set( COMPILE_WITH_AIFF_DECODING   DONT_COMPILE)
endif()

if( NOT NIMEDIA_ENABLE_AIFF_ENCODING )
  set( COMPILE_WITH_AIFF_ENCODING   DONT_COMPILE)
endif()

if( NOT NIMEDIA_ENABLE_AIFF_DECODING AND NOT NIMEDIA_ENABLE_AIFF_ENCODING )
  set( COMPILE_WITH_AIFF              DONT_COMPILE)
endif()

if( NOT NIMEDIA_ENABLE_FLAC_DECODING )
  set( COMPILE_WITH_FLAC_DECODING     DONT_COMPILE)
endif()

if( NOT NIMEDIA_ENABLE_OGG_DECODING )
  set( COMPILE_WITH_OGG_DECODING      DONT_COMPILE)
endif()

if( NOT NIMEDIA_ENABLE_MP3_DECODING )
  set( COMPILE_WITH_MP3_DECODING      DONT_COMPILE)
endif()

if( NOT NIMEDIA_ENABLE_MP4_DECODING )
  set( COMPILE_WITH_MP4_DECODING      DONT_COMPILE)
endif()

if( NOT NIMEDIA_ENABLE_WAV_DECODING )
  set( COMPILE_WITH_WAV_DECODING      DONT_COMPILE)
endif()

if( NOT NIMEDIA_ENABLE_WMA_DECODING )
  set( COMPILE_WITH_WMA_DECODING      DONT_COMPILE)
endif()

if( NOT NIMEDIA_ENABLE_WAV_ENCODING )
  set( COMPILE_WITH_WAV_ENCODING      DONT_COMPILE)
endif()

if( NOT NIMEDIA_ENABLE_WAV_DECODING AND NOT NIMEDIA_ENABLE_WAV_ENCODING )
  set( COMPILE_WITH_WAV               DONT_COMPILE)
endif()

if( NOT NIMEDIA_ENABLE_ITUNES_DECODING )
  set( COMPILE_WITH_AVASSETREADER     DONT_COMPILE)
endif()

set( COMPILE_WITH_COREAUDIO         DONT_COMPILE)
set( COMPILE_WITH_MEDIA_FOUNDATION  DONT_COMPILE)

#-----------------------------------------------------------------------------------------------------------------------
# dependencies
#-----------------------------------------------------------------------------------------------------------------------

if( NIMEDIA_ENABLE_MP3_DECODING OR NIMEDIA_ENABLE_MP4_DECODING OR NIMEDIA_ENABLE_WMA_DECODING)

  if( APPLE )

    set(COMPILE_WITH_COREAUDIO)

    if( NIMEDIA_ENABLE_WMA_DECODING)
      message(FATAL_ERROR
        "You are building ni-media with WMA support but this decoder is not supported on your platform\n"
        "You can build ni-media without WMA support by setting:\n"
        " * NIMEDIA_ENABLE_WMA_DECODING = OFF\n")
    endif()

    find_library(COREAUDIOTOOLBOX_FRAMEWORK AudioToolbox)
    if ( NOT COREAUDIOTOOLBOX_FRAMEWORK )
      message(FATAL_ERROR
        "You are building ni-media with MP3 / MP4 support but some required dependencies could not be found\n"
        "Make sure the AudioToolbox Framework  or disable MP3 and MP4 decoding by setting:\n"
        " * NIMEDIA_ENABLE_MP3_DECODING = OFF\n"
        " * NIMEDIA_ENABLE_MP4_DECODING = OFF\n")
    endif()

    list(APPEND codec_libraries ${COREAUDIOTOOLBOX_FRAMEWORK})

    if(NOT IOS)

      find_library(CORESERVICES_FRAMEWORK CoreServices)
      if ( NOT CORESERVICES_FRAMEWORK )
        message(FATAL_ERROR
          "You are building ni-media with MP3 / MP4 support but some required dependencies could not be found\n"
          "Make sure the CoreServices Framework can be found or disable MP3 and MP4 decoding by setting:\n"
          " * NIMEDIA_ENABLE_MP3_DECODING = OFF\n"
          " * NIMEDIA_ENABLE_MP4_DECODING = OFF\n")
      endif()

      list(APPEND codec_libraries ${CORESERVICES_FRAMEWORK})

    else()

      foreach( lib IN ITEMS Foundation AVFoundation CoreMedia )
        find_library(${lib}_FRAMEWORK ${lib})
        if( NOT ${lib}_FRAMEWORK )
          message(FATAL_ERROR "could not locate ${lib}.framework")
        endif()
        list(APPEND codec_libraries ${${lib}_FRAMEWORK})
      endforeach()

    endif()

  elseif( WIN32 )

    set(COMPILE_WITH_MEDIA_FOUNDATION)

    list(APPEND codec_libraries mfplat.lib mfreadwrite.lib mfuuid.lib Propsys.lib)

  else()

    message(FATAL_ERROR
      "You are building ni-media with MP3 / MP4 / WMA support but these decoders are not supported on your platform\n"
      "You can build ni-media without MP3 / MP4 / WMA support by setting:\n"
      " * NIMEDIA_ENABLE_MP3_DECODING = OFF\n"
      " * NIMEDIA_ENABLE_MP4_DECODING = OFF\n"
      " * NIMEDIA_ENABLE_WMA_DECODING = OFF\n" )
  endif()

endif()


if( NIMEDIA_ENABLE_FLAC_DECODING )

  if( NIMEDIA_OPEN_SOURCE )
    find_package(FLAC REQUIRED)
  else()
    find_package(NIFLAC REQUIRED)
    add_library(FLAC::flacpp ALIAS flacpp)
  endif()

  if ( NOT TARGET FLAC::flacpp )
    message(FATAL_ERROR
      "You are building ni-media with FLAC decoding support but the required flacpp library was not found\n"
      "Make sure library can be found or disable FLAC decoding by setting:\n"
      " * NIMEDIA_ENABLE_FLAC_DECODING = OFF\n")
  endif()
  list(APPEND codec_libraries FLAC::flacpp)
endif()


if ( NIMEDIA_ENABLE_OGG_DECODING )

  if( NIMEDIA_OPEN_SOURCE )
    find_package(Vorbis REQUIRED)
  else()
    find_package(NILibVorbis REQUIRED)
    add_library(Vorbis::vorbisfile ALIAS libvorbis)
  endif()

  if ( NOT TARGET Vorbis::vorbisfile)
    message(FATAL_ERROR
      "You are building ni-media with OGG decoding support but the required vorbisfile library was not found\n"
      "Make sure vorbisfile can be found or disable OGG decoding by setting:\n"
      " * NIMEDIA_ENABLE_OGG_DECODING = OFF\n")
  endif()
  list(APPEND codec_libraries Vorbis::vorbisfile)
endif()


#-----------------------------------------------------------------------------------------------------------------------
# sources
#-----------------------------------------------------------------------------------------------------------------------

add_src_file( Sources "inc/ni/media/iostreams/positioning.h"     )
add_src_file( Sources "src/ni/media/iostreams/stream_buffer.h"   )
add_src_file( Sources "src/ni/media/iostreams/fetch.h"           )
add_src_file( Sources "src/ni/media/iostreams/write_obj.h"       )
add_src_file( Sources "src/ni/media/iostreams/device/custom.h"      )
add_src_file( Sources "src/ni/media/iostreams/device/subview.h"     )
add_src_file( Sources "src/ni/media/iostreams/device/container.h"   )
add_src_file( Sources "src/ni/media/audio/fourcc.h"                                )
add_src_file( Sources "src/ni/media/audio/ieee80.h"                                )
add_src_file( Sources "src/ni/media/audio/iotools.cpp"          WITH_HEADER        )
add_src_file( Sources "src/ni/media/audio/stream_info.cpp"      WITH_PUBLIC_HEADER )
add_src_file( Sources "src/ni/media/audio/istream.cpp"          WITH_PUBLIC_HEADER )
add_src_file( Sources "src/ni/media/audio/ostream.cpp"          WITH_PUBLIC_HEADER )
add_src_file( Sources "inc/ni/media/audio/istream_info.h"                          )
add_src_file( Sources "inc/ni/media/audio/istream_source.h"                        )
add_src_file( Sources "inc/ni/media/audio/ostream_info.h"                          )
add_src_file( Sources "src/ni/media/audio/ivectorstream.cpp"    WITH_PUBLIC_HEADER )
add_src_file( Sources "src/ni/media/audio/ifstream.cpp"         WITH_PUBLIC_HEADER )
add_src_file( Sources "src/ni/media/audio/ofstream.cpp"         WITH_PUBLIC_HEADER )
add_src_file( Sources "src/ni/media/audio/ifvectorstream.cpp"   WITH_PUBLIC_HEADER )
add_src_file( Sources "src/ni/media/audio/fstream_info.cpp"     WITH_PUBLIC_HEADER )
add_src_file( Sources "src/ni/media/audio/ifstream_info.cpp"    WITH_PUBLIC_HEADER )
add_src_file( Sources "inc/ni/media/audio/ifstream_support.h"                      )
add_src_file( Sources "src/ni/media/audio/ofstream_info.cpp"    WITH_PUBLIC_HEADER )
add_src_file( Sources "src/ni/media/audio/source.h"                                )
add_src_file( Sources "src/ni/media/audio/source_impl.h"                           )
add_src_file( Sources "src/ni/media/audio/sink.h"                                  )
add_src_file( Sources "inc/ni/media/audio/streambuf.h"                             )
add_src_file( Sources "inc/ni/media/audio/custom_backend_source.h"                     )
add_src_file( Sources "inc/ni/media/audio/aiff/aiff_ifstream_info.h"  ${COMPILE_WITH_AIFF_DECODING} )
add_src_file( Sources "inc/ni/media/audio/aiff/aiff_ofstream_info.h"  ${COMPILE_WITH_AIFF_ENCODING} )
add_src_file( Sources "inc/ni/media/audio/aiff/aiff_specific_info.h"  ${COMPILE_WITH_AIFF}          )
add_src_file( Sources "src/ni/media/audio/aiff/aiff_source.h"         ${COMPILE_WITH_AIFF_DECODING} )
add_src_file( Sources "src/ni/media/audio/aiff/aiff_file_source.h"    ${COMPILE_WITH_AIFF_DECODING} )
add_src_file( Sources "src/ni/media/audio/aiff/aiff_vector_source.h"  ${COMPILE_WITH_AIFF_DECODING} )
add_src_file( Sources "src/ni/media/audio/aiff/aiff_custom_source.h"  ${COMPILE_WITH_AIFF_DECODING} )
add_src_file( Sources "src/ni/media/audio/aiff/aiff_chunks.h"         ${COMPILE_WITH_AIFF}          )
add_src_file( Sources "src/ni/media/audio/aiff/aiff_ofstream.cpp"     ${COMPILE_WITH_AIFF_ENCODING} WITH_PUBLIC_HEADER )
add_src_file( Sources "src/ni/media/audio/aiff/aiff_file_sink.h"      ${COMPILE_WITH_AIFF_ENCODING} )
add_src_file( Sources "src/ni/media/audio/aiff/aiff_sink.h"           ${COMPILE_WITH_AIFF_ENCODING} )
add_src_file( Sources "src/ni/media/audio/flac/flac_source.h"            ${COMPILE_WITH_FLAC_DECODING} )
add_src_file( Sources "src/ni/media/audio/flac/flac_file_source.h"       ${COMPILE_WITH_FLAC_DECODING} )
add_src_file( Sources "src/ni/media/audio/flac/flac_vector_source.h"     ${COMPILE_WITH_FLAC_DECODING} )
add_src_file( Sources "src/ni/media/audio/flac/flac_custom_source.h"     ${COMPILE_WITH_FLAC_DECODING} )
add_src_file( Sources "src/ni/media/audio/iostreams/container_source.h" )
add_src_file( Sources "src/ni/media/audio/iostreams/container_sink.h"   )
add_src_file( Sources "src/ni/media/audio/mp3/mp3_file_source.h"       ${COMPILE_WITH_MP3_DECODING} )
add_src_file( Sources "src/ni/media/audio/mp3/mp3_vector_source.h"     ${COMPILE_WITH_MP3_DECODING} )
add_src_file( Sources "src/ni/media/audio/mp3/mp3_custom_source.h"     ${COMPILE_WITH_MP3_DECODING} )
add_src_file( Sources "src/ni/media/audio/mp4/mp4_file_source.h"       ${COMPILE_WITH_MP4_DECODING} )
add_src_file( Sources "src/ni/media/audio/mp4/mp4_vector_source.h"     ${COMPILE_WITH_MP4_DECODING} )
add_src_file( Sources "src/ni/media/audio/mp4/mp4_custom_source.h"     ${COMPILE_WITH_MP4_DECODING} )
add_src_file( Sources "src/ni/media/audio/os/os_source.h" )
add_src_file( Sources "src/ni/media/audio/os/mac/core_audio_source.h"  ${COMPILE_WITH_COREAUDIO} )
add_src_file( Sources "src/ni/media/audio/os/win/media_foundation_helper.h"  ${COMPILE_WITH_MEDIA_FOUNDATION} )
add_src_file( Sources "src/ni/media/audio/os/win/media_foundation_source.h"  ${COMPILE_WITH_MEDIA_FOUNDATION} )
add_src_file( Sources "src/ni/media/audio/os/ios/avassetreader_source.mm"  ${COMPILE_WITH_AVASSETREADER} WITH_HEADER)
add_src_file( Sources "src/ni/media/audio/ogg/ogg_source.h"            ${COMPILE_WITH_OGG_DECODING} )
add_src_file( Sources "src/ni/media/audio/ogg/ogg_file_source.h"       ${COMPILE_WITH_OGG_DECODING} )
add_src_file( Sources "src/ni/media/audio/ogg/ogg_vector_source.h"     ${COMPILE_WITH_OGG_DECODING} )
add_src_file( Sources "src/ni/media/audio/ogg/ogg_custom_source.h"     ${COMPILE_WITH_OGG_DECODING} )
add_src_file( Sources "inc/ni/media/audio/wav/wav_specific_info.h"     ${COMPILE_WITH_WAV} )
add_src_file( Sources "src/ni/media/audio/wav/wav_chunks.h"            ${COMPILE_WITH_WAV} )
add_src_file( Sources "src/ni/media/audio/wav/wav_format.h"            ${COMPILE_WITH_WAV} )
add_src_file( Sources "src/ni/media/audio/wav/wav_source.h"            ${COMPILE_WITH_WAV_DECODING} )
add_src_file( Sources "src/ni/media/audio/wav/wav_file_source.h"       ${COMPILE_WITH_WAV_DECODING} )
add_src_file( Sources "src/ni/media/audio/wav/wav_vector_source.h"     ${COMPILE_WITH_WAV_DECODING} )
add_src_file( Sources "src/ni/media/audio/wav/wav_custom_source.h"     ${COMPILE_WITH_WAV_DECODING} )
add_src_file( Sources "inc/ni/media/audio/wav/wav_ifstream_info.h"     ${COMPILE_WITH_WAV_DECODING} )
add_src_file( Sources "src/ni/media/audio/wav/wav_file_sink.h"         ${COMPILE_WITH_WAV_ENCODING} )
add_src_file( Sources "inc/ni/media/audio/wav/wav_ofstream_info.h"     ${COMPILE_WITH_WAV_ENCODING} )
add_src_file( Sources "src/ni/media/audio/wav/wav_ofstream.cpp"        ${COMPILE_WITH_WAV_ENCODING}  WITH_PUBLIC_HEADER )
add_src_file( Sources "src/ni/media/audio/wma/wma_file_source.h"       ${COMPILE_WITH_WMA_DECODING} )
add_src_file( Sources "src/ni/media/audio/wma/wma_vector_source.h"     ${COMPILE_WITH_WMA_DECODING} )
add_src_file( Sources "src/ni/media/audio/wma/wma_custom_source.h"     ${COMPILE_WITH_WMA_DECODING} )

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${Sources})

#-----------------------------------------------------------------------------------------------------------------------
# linking
#-----------------------------------------------------------------------------------------------------------------------

add_library                 ( audiostream ${Sources} )
target_include_directories  ( audiostream
                                PUBLIC
                                  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
                                  $<INSTALL_INTERFACE:include>
                                PRIVATE
                                  src
                            )

if (NIMEDIA_UNITY_BUILDS)
  set_target_properties( audiostream PROPERTIES
    UNITY_BUILD ON
    UNITY_BUILD_BATCH_SIZE 16
  )
endif()

target_link_libraries       ( audiostream PUBLIC pcm
                                          PRIVATE Boost::iostreams Boost::filesystem Boost::system ${codec_libraries})

if(MSVC)
  target_compile_options( audiostream PRIVATE /wd4351 )  # MSVC new behaviour warning
endif()

target_compile_definitions( audiostream PRIVATE
  NIMEDIA_ENABLE_AIFF_DECODING=$<BOOL:${NIMEDIA_ENABLE_AIFF_DECODING}>
  NIMEDIA_ENABLE_FLAC_DECODING=$<BOOL:${NIMEDIA_ENABLE_FLAC_DECODING}>
  NIMEDIA_ENABLE_MP3_DECODING=$<BOOL:${NIMEDIA_ENABLE_MP3_DECODING}>
  NIMEDIA_ENABLE_MP4_DECODING=$<BOOL:${NIMEDIA_ENABLE_MP4_DECODING}>
  NIMEDIA_ENABLE_OGG_DECODING=$<BOOL:${NIMEDIA_ENABLE_OGG_DECODING}>
  NIMEDIA_ENABLE_WAV_DECODING=$<BOOL:${NIMEDIA_ENABLE_WAV_DECODING}>
  NIMEDIA_ENABLE_WMA_DECODING=$<BOOL:${NIMEDIA_ENABLE_WMA_DECODING}>
  NIMEDIA_ENABLE_WAV_ENCODING=$<BOOL:${NIMEDIA_ENABLE_WAV_ENCODING}>
  NIMEDIA_ENABLE_ITUNES_DECODING=$<BOOL:${NIMEDIA_ENABLE_ITUNES_DECODING}>
  )

#-----------------------------------------------------------------------------------------------------------------------
# install
#-----------------------------------------------------------------------------------------------------------------------

if(NOT DISABLE_INSTALL_HEADERS)
  install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/inc/
    DESTINATION include
  )
endif()

if(NOT DISABLE_INSTALL_LIBS)
  install(
    TARGETS audiostream
    EXPORT ni-mediaExport
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
  )
endif()

#-----------------------------------------------------------------------------------------------------------------------
# testing
#-----------------------------------------------------------------------------------------------------------------------

include(CMakeDependentOption)
cmake_dependent_option(NIMEDIA_AUDIOSTREAM_TESTS "Enable audiostream tests" ON "NIMEDIA_TESTS" OFF)

if(NIMEDIA_AUDIOSTREAM_TESTS)
  add_subdirectory(test)
endif()
