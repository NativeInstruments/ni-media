cmake_minimum_required(VERSION 3.12.0)

if( TARGET ni-media )
  return()
endif()

project( ni-media
  VERSION 0.1.0
)

option( NIMEDIA_OPEN_SOURCE                   "Build ni-media open source version"    ON )
option( NIMEDIA_TREAT_WARNINGS_AS_ERRORS      "Treat compile warnings as errors"      OFF)

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
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/libs/ni-cmake-tools)
    include(ni-macros)
    ni_cmake_tools_register(libs/ni-cmake-tools)
    add_subdirectory(libs/nilibs nilibs-binary)
  else()
    add_subdirectory(../nilibs nilibs-binary)
    include( ni-macros )

    set(DISABLE_INSTALL_HEADERS ON)
    set(DISABLE_INSTALL_LIBS ON)
  endif()

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
    find_package(GTest "1.10.0" REQUIRED)
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

if( NOT NIMEDIA_OPEN_SOURCE )
  if( NI_CLANG_DEBUG )
    ni_target_add_clang_debug( audiostream )
    if( TARGET pcm_test )
      ni_target_add_clang_debug( pcm_test )
    endif()
  endif()
endif()

foreach( target audiostream audiostream_test pcm_test generator )
  if( TARGET ${target} )
    if( NIMEDIA_OPEN_SOURCE )
      set_property( TARGET ${target} PROPERTY CXX_STANDARD 17)
      if( NIMEDIA_TREAT_WARNINGS_AS_ERRORS )
        ni_treat_warnings_as_errors( ${target} )
      endif()
    endif()
    set_property( TARGET ${target} PROPERTY FOLDER ni-media )
  endif()
endforeach()


if( NIMEDIA_OPEN_SOURCE )
  
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
endif()
