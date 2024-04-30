###
### this file is private to the Native Instruments build system, please ignore
###

if (TARGET ni-media)
  return()
endif()

### set package path to current path if not set by nib.
###
### we have to define and use a local function here so that CMAKE_CURRENT_FUNCTION_LIST_DIR is set.
function( pr_init_ni_nimedia_package_path )
  if( NOT DEFINED NI_NIMEDIA_PACKAGE_PATH )
    set( NI_NIMEDIA_PACKAGE_PATH ${CMAKE_CURRENT_FUNCTION_LIST_DIR} PARENT_SCOPE )
  endif()
endfunction()
pr_init_ni_nimedia_package_path()

set( NIMEDIA_PCM_INCLUDE_DIRS "${NI_NIMEDIA_PACKAGE_PATH}/pcm/inc" )

if ( NOT (EXISTS ${NIMEDIA_PCM_INCLUDE_DIRS}) )
  message( SEND_ERROR "Cannot find ni-media pcm headers" )
endif()

set( NIMEDIA_AUDIOSTREAM_INCLUDE_DIRS "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/inc" )

if ( NOT (EXISTS ${NIMEDIA_AUDIOSTREAM_INCLUDE_DIRS}) )
  message( SEND_ERROR "Cannot find ni-media audiostream headers" )
endif()

find_package( NIBoost REQUIRED )

add_library                 ( pcm INTERFACE )
target_include_directories  ( pcm INTERFACE "${NIMEDIA_PCM_INCLUDE_DIRS}")
file( GLOB_RECURSE pcm_headers ${NIMEDIA_PCM_INCLUDE_DIRS}/ni/media/pcm/*h )
target_sources              ( pcm INTERFACE ${pcm_headers} )
target_link_libraries       ( pcm INTERFACE boost_header_only_interface )

add_library                 ( audiostream )
target_include_directories  ( audiostream
                                PUBLIC "${NIMEDIA_AUDIOSTREAM_INCLUDE_DIRS}"
                                PRIVATE "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src"
                            )
set( audiostream_headers 
  "${NIMEDIA_AUDIOSTREAM_INCLUDE_DIRS}/ni/media/audio/aiff/aiff_ifstream_info.h"
  "${NIMEDIA_AUDIOSTREAM_INCLUDE_DIRS}/ni/media/audio/aiff/aiff_ofstream_info.h"
  "${NIMEDIA_AUDIOSTREAM_INCLUDE_DIRS}/ni/media/audio/aiff/aiff_ofstream.h"
  "${NIMEDIA_AUDIOSTREAM_INCLUDE_DIRS}/ni/media/audio/aiff/aiff_specific_info.h"
  "${NIMEDIA_AUDIOSTREAM_INCLUDE_DIRS}/ni/media/audio/custom_backend_source.h"
  "${NIMEDIA_AUDIOSTREAM_INCLUDE_DIRS}/ni/media/audio/fstream_info.h"
  "${NIMEDIA_AUDIOSTREAM_INCLUDE_DIRS}/ni/media/audio/ifstream_info.h"
  "${NIMEDIA_AUDIOSTREAM_INCLUDE_DIRS}/ni/media/audio/ifstream_support.h"
  "${NIMEDIA_AUDIOSTREAM_INCLUDE_DIRS}/ni/media/audio/ifstream.h"
  "${NIMEDIA_AUDIOSTREAM_INCLUDE_DIRS}/ni/media/audio/ifvectorstream.h"
  "${NIMEDIA_AUDIOSTREAM_INCLUDE_DIRS}/ni/media/audio/istream_info.h"
  "${NIMEDIA_AUDIOSTREAM_INCLUDE_DIRS}/ni/media/audio/istream_source.h"
  "${NIMEDIA_AUDIOSTREAM_INCLUDE_DIRS}/ni/media/audio/istream.h"
  "${NIMEDIA_AUDIOSTREAM_INCLUDE_DIRS}/ni/media/audio/ivectorstream.h"
  "${NIMEDIA_AUDIOSTREAM_INCLUDE_DIRS}/ni/media/audio/ofstream_info.h"
  "${NIMEDIA_AUDIOSTREAM_INCLUDE_DIRS}/ni/media/audio/ofstream.h"
  "${NIMEDIA_AUDIOSTREAM_INCLUDE_DIRS}/ni/media/audio/ostream_info.h"
  "${NIMEDIA_AUDIOSTREAM_INCLUDE_DIRS}/ni/media/audio/ostream.h"
  "${NIMEDIA_AUDIOSTREAM_INCLUDE_DIRS}/ni/media/audio/stream_info.h"
  "${NIMEDIA_AUDIOSTREAM_INCLUDE_DIRS}/ni/media/audio/streambuf.h"
  "${NIMEDIA_AUDIOSTREAM_INCLUDE_DIRS}/ni/media/audio/wav/wav_ifstream_info.h"
  "${NIMEDIA_AUDIOSTREAM_INCLUDE_DIRS}/ni/media/audio/wav/wav_ofstream_info.h"
  "${NIMEDIA_AUDIOSTREAM_INCLUDE_DIRS}/ni/media/audio/wav/wav_ofstream.h"
  "${NIMEDIA_AUDIOSTREAM_INCLUDE_DIRS}/ni/media/audio/wav/wav_specific_info.h"
  "${NIMEDIA_AUDIOSTREAM_INCLUDE_DIRS}/ni/media/iostreams/positioning.h"
)
set( audiostream_sources
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/aiff/aiff_chunks.h"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/aiff/aiff_custom_source.h"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/aiff/aiff_file_sink.h"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/aiff/aiff_file_source.h"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/aiff/aiff_ofstream.cpp"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/aiff/aiff_sink.h"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/aiff/aiff_source.h"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/aiff/aiff_vector_source.h"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/flac/flac_custom_source.h"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/flac/flac_file_source.h"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/flac/flac_source.h"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/flac/flac_vector_source.h"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/fourcc.h"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/fstream_info.cpp"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/ieee80.h"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/ifstream_info.cpp"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/ifstream.cpp"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/ifvectorstream.cpp"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/iostreams/container_sink.h"  
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/iostreams/container_source.h"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/iotools.cpp"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/iotools.h"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/istream.cpp"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/ivectorstream.cpp"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/ofstream_info.cpp"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/ofstream.cpp"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/ogg/ogg_custom_source.h"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/ogg/ogg_file_source.h"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/ogg/ogg_source.h"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/ogg/ogg_vector_source.h"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/os/os_source.h"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/ostream.cpp"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/sink.h"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/source_impl.h"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/source.h"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/stream_info.cpp"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/wav/wav_chunks.h"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/wav/wav_custom_source.h"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/wav/wav_file_sink.h"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/wav/wav_file_source.h"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/wav/wav_format.h"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/wav/wav_ofstream.cpp"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/wav/wav_source.h"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/wav/wav_vector_source.h"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/iostreams/device/container.h"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/iostreams/device/custom.h"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/iostreams/device/subview.h"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/iostreams/fetch.h"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/iostreams/stream_buffer.h"
  "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/iostreams/write_obj.h"
)
if ( APPLE OR WIN32 )
  list( APPEND audiostream_sources
    "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/mp3/mp3_file_source.h"
    "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/mp3/mp3_vector_source.h"
    "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/mp3/mp3_custom_source.h"
    "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/mp4/mp4_file_source.h"
    "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/mp4/mp4_vector_source.h"
    "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/mp4/mp4_custom_source.h"
  )
endif()
if ( APPLE )
  list( APPEND audiostream_sources
    "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/os/mac/core_audio_source.h"
  )
endif()
if ( WIN32 )
  list( APPEND audiostream_sources
    "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/os/win/media_foundation_helper.h"
    "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/os/win/media_foundation_source.h"
  )
endif()
if ( IOS )
  list( APPEND audiostream_sources
    "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/os/ios/avassetreader_source.h"
    "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/os/ios/avassetreader_source.mm"
  )
endif()
if ( WIN32 AND NIMEDIA_ENABLE_WMA_DECODING )
  list( APPEND audiostream_sources
    "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/wma/wma_custom_source.h"
    "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/wma/wma_file_source.h"
    "${NI_NIMEDIA_PACKAGE_PATH}/audiostream/src/ni/media/audio/wma/wma_vector_source.h"
  )
endif()
target_sources( audiostream PUBLIC ${audiostream_headers} PRIVATE ${audiostream_sources})

# codec libraries
find_package( NIFLAC REQUIRED )
list( APPEND codec_libraries flacpp )
find_package( NILibVorbis REQUIRED )
list( APPEND codec_libraries libvorbis )
if ( APPLE )
  if ( NOT IOS )
    find_library( CORESERVICES_FRAMEWORK CoreServices )
    list( APPEND codec_libraries ${CORESERVICES_FRAMEWORK} )
  else()
    foreach( lib IN ITEMS Foundation AVFoundation CoreMedia )
      find_library( ${lib}_FRAMEWORK ${lib} )
      if ( NOT ${lib}_FRAMEWORK )
        message( FATAL_ERROR "could not locate ${lib}.framework" )
      endif()
      list( APPEND codec_libraries ${${lib}_FRAMEWORK} )
    endforeach()
  endif()
endif()
if ( WIN32 )
  list( APPEND codec_libraries mfplat.lib mfreadwrite.lib mfuuid.lib Propsys.lib )
endif()

target_link_libraries( audiostream PUBLIC pcm
                                   PRIVATE boost_iostreams boost_filesystem boost_system ${codec_libraries}
                     )

target_compile_definitions( audiostream PRIVATE
  NIMEDIA_ENABLE_AIFF_DECODING=1
  NIMEDIA_ENABLE_FLAC_DECODING=1
  NIMEDIA_ENABLE_MP3_DECODING=1
  NIMEDIA_ENABLE_MP4_DECODING=1
  NIMEDIA_ENABLE_OGG_DECODING=1
  NIMEDIA_ENABLE_WAV_DECODING=1
  NIMEDIA_ENABLE_WMA_DECODING=$<BOOL:${NIMEDIA_ENABLE_WMA_DECODING}> #wma support is off by default, clients need to turn it on
  NIMEDIA_ENABLE_WAV_ENCODING=1
  NIMEDIA_ENABLE_ITUNES_DECODING=1
)

add_library( ni-media INTERFACE )
target_link_libraries( ni-media INTERFACE audiostream pcm )

add_library( ni::media ALIAS ni-media )
