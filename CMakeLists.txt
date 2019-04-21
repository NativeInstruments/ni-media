cmake_minimum_required(VERSION 3.8)


if( TARGET ni-media )
  return()
endif()

project( ni-media )

option( NIMEDIA_TREAT_WARNINGS_AS_ERRORS      "Treat compile warnings as errors"      OFF)
option( NIMEDIA_ENABLE_DEPRECATION_WARNINGS   "Enable ni-media deprecation warnings"  ON )
if( CMAKE_PROJECT_NAME STREQUAL "ni-media" )
  option( NIMEDIA_TESTS "Build ni-media Tests" ON )
else()
  option( NIMEDIA_TESTS "Build ni-media Tests" OFF )
endif()


if(WIN32 AND CMAKE_CL_64)
  set(WIN64 1)
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
  set(LINUX 1)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(ni_add_src_file)
include(ni_add_src_group)
include(ni_add_gtest)
include(ni_treat_warnings_as_errors)

if( NIMEDIA_TESTS )
  enable_testing()
  include(ni_find_gtest)
  ni_find_gtest()
endif()

add_subdirectory( pcm )
add_subdirectory( audiostream )

install(
    EXPORT ni-mediaExport
    FILE ni-mediaConfig.cmake
    NAMESPACE ni::
    DESTINATION share/ni-media
)

# for backwards compatibility:
add_library( ni-media INTERFACE )
target_link_libraries( ni-media INTERFACE audiostream pcm )
if( NOT NIMEDIA_ENABLE_DEPRECATION_WARNINGS )
  target_compile_definitions( ni-media INTERFACE NI_MEDIA_NO_DEPRECATION_WARNINGS )
endif()


foreach( target audiostream audiostream_test pcm pcm_test )
  if( TARGET ${target} )
    if( NIMEDIA_TREAT_WARNINGS_AS_ERRORS )
      ni_treat_warnings_as_errors( ${target} )
    endif()

    set_property( TARGET ${target} PROPERTY FOLDER ni-media )
    target_compile_features(${target} PRIVATE cxx_std_14)
  endif()
endforeach()

