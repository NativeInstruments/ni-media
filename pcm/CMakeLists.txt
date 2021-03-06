
#--------------------------------------------------------------------
# sources
#--------------------------------------------------------------------

add_src_file  (FILES_media_pcm "${CMAKE_CURRENT_SOURCE_DIR}/inc/ni/media/pcm/converter.h")
add_src_file  (FILES_media_pcm "${CMAKE_CURRENT_SOURCE_DIR}/inc/ni/media/pcm/description.h")
add_src_file  (FILES_media_pcm "${CMAKE_CURRENT_SOURCE_DIR}/inc/ni/media/pcm/dispatch.h")
add_src_file  (FILES_media_pcm "${CMAKE_CURRENT_SOURCE_DIR}/inc/ni/media/pcm/compiletime_format.h")
add_src_file  (FILES_media_pcm "${CMAKE_CURRENT_SOURCE_DIR}/inc/ni/media/pcm/runtime_format.h")
add_src_file  (FILES_media_pcm "${CMAKE_CURRENT_SOURCE_DIR}/inc/ni/media/pcm/format.h" )
add_src_file  (FILES_media_pcm "${CMAKE_CURRENT_SOURCE_DIR}/inc/ni/media/pcm/iterator.h")
add_src_file  (FILES_media_pcm "${CMAKE_CURRENT_SOURCE_DIR}/inc/ni/media/pcm/algorithm.h")
add_src_file  (FILES_media_pcm "${CMAKE_CURRENT_SOURCE_DIR}/inc/ni/media/pcm/limits.h")
add_src_group (FILES_All media_pcm FILES_media_pcm)

add_src_file  (FILES_media_pcm_detail "${CMAKE_CURRENT_SOURCE_DIR}/inc/ni/media/pcm/detail/tuple_find.h")
add_src_file  (FILES_media_pcm_detail "${CMAKE_CURRENT_SOURCE_DIR}/inc/ni/media/pcm/detail/tuple_to_array.h")
add_src_group (FILES_All media_pcm_detail FILES_media_pcm_detail)

add_src_file  (FILES_media_pcm_range "${CMAKE_CURRENT_SOURCE_DIR}/inc/ni/media/pcm/range/converted.h")
add_src_group (FILES_All media_pcm_range FILES_media_pcm_range)

add_src_file  (FILES_media_pcm_algorithm "${CMAKE_CURRENT_SOURCE_DIR}/inc/ni/media/pcm/algorithm/copy.h")
add_src_file  (FILES_media_pcm_algorithm "${CMAKE_CURRENT_SOURCE_DIR}/inc/ni/media/pcm/algorithm/copy_n.h")
add_src_group (FILES_All media_pcm_algorithm FILES_media_pcm_algorithm)


#--------------------------------------------------------------------
# linking
#--------------------------------------------------------------------

add_library                 ( pcm INTERFACE )
target_include_directories  ( pcm INTERFACE
                                  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc> 
                                  $<INSTALL_INTERFACE:include>
                            )

target_sources              ( pcm INTERFACE "$<BUILD_INTERFACE:${FILES_All}>" )

target_link_libraries       ( pcm INTERFACE Boost::boost )


#-----------------------------------------------------------------------------------------------------------------------
# install
#-----------------------------------------------------------------------------------------------------------------------

if(NOT DISABLE_INSTALL_HEADERS)
  install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/inc/
    DESTINATION include
  )
endif()

install(
  TARGETS pcm
  EXPORT ni-mediaExport
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

#--------------------------------------------------------------------
# testing
#--------------------------------------------------------------------

include(CMakeDependentOption)
cmake_dependent_option(NIMEDIA_PCM_TESTS "Enable pcm tests" ON "NIMEDIA_TESTS" OFF)

if(NIMEDIA_PCM_TESTS)
  add_subdirectory(test)
endif()

