cmake_minimum_required(VERSION 3.16.0)

if( TARGET ni-media )
  return()
endif()

project( ni-media
  VERSION 0.1.0
)

option( NIMEDIA_TREAT_WARNINGS_AS_ERRORS "Treat compile warnings as errors" OFF )
option( NIMEDIA_UNITY_BUILDS             "Build ni-media With Unity Builds" ON  )

if( CMAKE_PROJECT_NAME STREQUAL "ni-media" )
  option( NIMEDIA_TESTS "Build ni-media Tests" ON )
else()
  option( NIMEDIA_TESTS "Build ni-media Tests" OFF )
endif()

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

if( NIMEDIA_TESTS )
  enable_testing()
  find_package(GTest "1.10.0" REQUIRED)
endif()

add_subdirectory( pcm )
add_subdirectory( audiostream )

# for backwards compatibility:
add_library( ni-media INTERFACE )
target_link_libraries( ni-media INTERFACE audiostream pcm )

foreach( target audiostream audiostream_test pcm_test generator )
  if( TARGET ${target} )
    set_property( TARGET ${target} PROPERTY CXX_STANDARD 14)
    if( NIMEDIA_TREAT_WARNINGS_AS_ERRORS )
      ni_treat_warnings_as_errors( ${target} )
    endif()
    set_property( TARGET ${target} PROPERTY FOLDER ni-media )
  endif()
endforeach()


include(CMakePackageConfigHelpers)

set(CONFIG_DESTINATION lib/cmake/ni-media)

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ni-mediaConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/ni-mediaConfig.cmake"
  INSTALL_DESTINATION ${CONFIG_DESTINATION}
)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/ni-mediaConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

install(
  EXPORT ni-mediaExport
  FILE ni-mediaTargets.cmake
  NAMESPACE ni::
  DESTINATION ${CONFIG_DESTINATION}
)

install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/ni-mediaConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/ni-mediaConfigVersion.cmake
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindFLAC.cmake
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindOgg.cmake
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindVorbis.cmake             
  DESTINATION
    ${CONFIG_DESTINATION}
)
