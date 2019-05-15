cmake_minimum_required(VERSION 3.2)
if(POLICY CMP0063)
  cmake_policy(SET CMP0063 NEW)
endif()

if( TARGET ni-media )
  return()
endif()

project( ni-media )

option( NIMEDIA_OPEN_SOURCE                   "Build ni-media open source version"    OFF)
option( NIMEDIA_ENABLE_DEPRECATION_WARNINGS   "Enable ni-media deprecation warnings"  ON )
if( CMAKE_PROJECT_NAME STREQUAL "ni-media" )
  option( NIMEDIA_TESTS "Build ni-media Tests" ON )
else()
  option( NIMEDIA_TESTS "Build ni-media Tests" OFF )
endif()

if( NIMEDIA_OPEN_SOURCE )

  if(WIN32 AND CMAKE_CL_64)
    set(WIN64 1)
  elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(LINUX 1)
  endif()

  list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

  include(ni_add_src_file)
  include(ni_add_src_group)
  include(ni_add_test)

else()

  if( CMAKE_PROJECT_NAME STREQUAL "ni-media" )
    set( NILIBS_LIGHT ON CACHE BOOL "Build NILibs without GUI libraries" FORCE )
    add_subdirectory(libs/nilibs nilibs-binary)
  else()
    add_subdirectory(../nilibs nilibs-binary)
  endif()

  include( ni-macros )

  option( NIMEDIA_UNITY_BUILDS "Build ni-media With Unity Builds" ON )

  if( NOT NIMEDIA_UNITY_BUILDS )
    set( SCOPED_DISABLE_UNITY_BUILDS 1 )
  endif()

endif()


if( NIMEDIA_TESTS )

  enable_testing()

  if( NIMEDIA_OPEN_SOURCE )
  	include(ni_find_gtest)
  	ni_find_gtest()
  else()
    find_package(NIGTest REQUIRED)
    add_library(GTest::GTest ALIAS gtest)
  endif()

endif()

add_subdirectory( pcm )
add_subdirectory( audiostream )

# for backwards compatibility:
add_library( ni-media INTERFACE )
target_link_libraries( ni-media INTERFACE audiostream pcm )
if( NOT NIMEDIA_ENABLE_DEPRECATION_WARNINGS )
  target_compile_definitions( ni-media INTERFACE NI_MEDIA_NO_DEPRECATION_WARNINGS )
endif()


foreach( target audiostream audiostream_test pcm pcm_test )
  if( TARGET ${target} )
    set_property( TARGET ${target} PROPERTY FOLDER ni-media )
  endif()
endforeach()

if( NIMEDIA_OPEN_SOURCE )
  foreach( target audiostream audiostream_test pcm pcm_test )
    if( TARGET ${target} )
      set_property( TARGET ${target} PROPERTY CXX_STANDARD 14)
    endif()
  endforeach()
endif()
