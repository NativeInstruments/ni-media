cmake_minimum_required(VERSION 3.2)

if( TARGET ni-media )
  return()
endif()

project( ni-media )

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


if( NIMEDIA_TESTS )
  enable_testing()
  find_package(GTest REQUIRED)
  # Workaroud: cmake < 3.6 does not create an imported target for GTest
  if(NOT TARGET GTest::GTest)
      add_library(GTest::GTest UNKNOWN IMPORTED)
      set_target_properties(GTest::GTest PROPERTIES
          INTERFACE_LINK_LIBRARIES "Threads::Threads")
      if(GTEST_INCLUDE_DIRS)
          set_target_properties(GTest::GTest PROPERTIES
              INTERFACE_INCLUDE_DIRECTORIES "${GTEST_INCLUDE_DIRS}")
      endif()
      if(EXISTS "${GTEST_LIBRARY}")
          set_target_properties(GTest::GTest PROPERTIES
              IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
              IMPORTED_LOCATION "${GTEST_LIBRARY}")
      endif()
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
    set_property( TARGET ${target} PROPERTY CXX_STANDARD 14)
  endif()
endforeach()

