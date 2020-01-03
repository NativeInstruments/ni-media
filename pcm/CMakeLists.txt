
#--------------------------------------------------------------------
# sources
#--------------------------------------------------------------------

add_src_file  (FILES_media_pcm "inc/ni/media/pcm/converter.h")
add_src_file  (FILES_media_pcm "inc/ni/media/pcm/description.h")
add_src_file  (FILES_media_pcm "inc/ni/media/pcm/dispatch.h")
add_src_file  (FILES_media_pcm "inc/ni/media/pcm/compiletime_format.h")
add_src_file  (FILES_media_pcm "inc/ni/media/pcm/runtime_format.h")
add_src_file  (FILES_media_pcm "inc/ni/media/pcm/format.h" )
add_src_file  (FILES_media_pcm "inc/ni/media/pcm/iterator.h")
add_src_file  (FILES_media_pcm "inc/ni/media/pcm/algorithm.h")
add_src_file  (FILES_media_pcm "inc/ni/media/pcm/limits.h")
add_src_file  (FILES_media_pcm "src/ni/media/pcm/dummy.cpp")
add_src_group (FILES_All media_pcm FILES_media_pcm)

add_src_file  (FILES_media_pcm_detail "inc/ni/media/pcm/detail/tuple_find.h")
add_src_file  (FILES_media_pcm_detail "inc/ni/media/pcm/detail/tuple_to_array.h")
add_src_group (FILES_All media_pcm_detail FILES_media_pcm_detail)

add_src_file  (FILES_media_pcm_range "inc/ni/media/pcm/range/converted.h")
add_src_group (FILES_All media_pcm_range FILES_media_pcm_range)

add_src_file  (FILES_media_pcm_algorithm "inc/ni/media/pcm/algorithm/copy.h")
add_src_file  (FILES_media_pcm_algorithm "inc/ni/media/pcm/algorithm/copy_n.h")
add_src_group (FILES_All media_pcm_algorithm FILES_media_pcm_algorithm)


#--------------------------------------------------------------------
# linking
#--------------------------------------------------------------------

add_library                 ( pcm EXCLUDE_FROM_ALL ${FILES_All} )
target_include_directories  ( pcm PUBLIC inc
                                  PRIVATE src )
target_link_libraries       ( pcm PUBLIC Boost::boost)

#--------------------------------------------------------------------
# testing
#--------------------------------------------------------------------

include(CMakeDependentOption)
cmake_dependent_option(NIMEDIA_PCM_TESTS "Enable pcm tests" ON "NIMEDIA_TESTS" OFF)

if(NIMEDIA_PCM_TESTS)
  add_subdirectory(test)
endif()

