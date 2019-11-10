cmake_minimum_required(VERSION 3.12.4)

if( TARGET ni-media )
  return()
endif()

project( ni-media )

option( NIMEDIA_OPEN_SOURCE                   "Build ni-media open source version"    ON )
option( NIMEDIA_TREAT_WARNINGS_AS_ERRORS      "Treat compile warnings as errors"      OFF)
option( NIMEDIA_ENABLE_DEPRECATION_WARNINGS   "Enable ni-media deprecation warnings"  ON )
if( CMAKE_PROJECT_NAME STREQUAL "ni-media" )
  option( NIMEDIA_TESTS "Build ni-media Tests" ON )
else()
  option( NIMEDIA_TESTS "Build ni-media Tests" OFF )
endif()

if( NIMEDIA_OPEN_SOURCE )

  list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

  include(ni_add_src_file)
  include(ni_add_src_group)
  include(ni_add_test)
  include(ni_treat_warnings_as_errors)
  
  if(WIN32 AND CMAKE_CL_64)
    set(WIN64 1)
  elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(LINUX 1)
  endif()

  if( CMAKE_BUILD_TYPE MATCHES Coverage )
    add_compile_options( -g -O0 --coverage )
    set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lgcov --coverage" )
    set( CMAKE_BUILD_TYPE Debug FORCE )
  endif()

  find_package(Boost "1.61.0" REQUIRED COMPONENTS iostreams filesystem system program_options regex)

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

  find_package(NIBoost REQUIRED)
  add_library(Boost::boost              ALIAS boost_header_only_interface)
  add_library(Boost::iostreams          ALIAS boost_iostreams)
  add_library(Boost::filesystem         ALIAS boost_filesystem)
  add_library(Boost::system             ALIAS boost_system)
  add_library(Boost::program_options    ALIAS boost_program_options)

endif()


if( NIMEDIA_TESTS )

  enable_testing()

  if( NIMEDIA_OPEN_SOURCE )
    find_package(GTest REQUIRED)
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


foreach( target audiostream audiostream_test pcm pcm_test generator )
  if( TARGET ${target} )
    if( NIMEDIA_OPEN_SOURCE )
      set_property( TARGET ${target} PROPERTY CXX_STANDARD 14)
      if( NIMEDIA_TREAT_WARNINGS_AS_ERRORS )
        ni_treat_warnings_as_errors( ${target} )
      endif()
    endif()
    set_property( TARGET ${target} PROPERTY FOLDER ni-media )
  endif()
endforeach()


install(
    EXPORT ni-mediaExport
    FILE ni-mediaConfig.cmake
    NAMESPACE ni::
    DESTINATION lib/cmake/ni-media
)

