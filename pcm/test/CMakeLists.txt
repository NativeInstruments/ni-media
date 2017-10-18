
add_src_file  ( FILES_main "main.cpp" )
add_src_group ( FILES_test_all test_main FILES_main )


#--------------------------------------------------------------------
# pcm

add_src_file  (FILES_test_pcm "ni/media/pcm/numspace.h"             DISABLE_COMPILE_HELPER)
add_src_file  (FILES_test_pcm "ni/media/pcm/format.test.cpp"        DISABLE_COMPILE_HELPER)
add_src_file  (FILES_test_pcm "ni/media/pcm/converter.test.cpp"     DISABLE_COMPILE_HELPER)
add_src_file  (FILES_test_pcm "ni/media/pcm/limits.test.cpp"        DISABLE_COMPILE_HELPER)
add_src_file  (FILES_test_pcm "ni/media/pcm/iterator.test.cpp"      DISABLE_COMPILE_HELPER)
add_src_file  (FILES_test_pcm "ni/media/pcm/iterator_copy.test.cpp" DISABLE_COMPILE_HELPER)

add_src_group (FILES_test_all test_pcm FILES_test_pcm)

#--------------------------------------------------------------------
# pcm detail

add_src_file  (FILES_test_pcm_detail "ni/media/pcm/detail/find_unique.test.cpp")
add_src_file  (FILES_test_pcm_detail "ni/media/pcm/detail/tuple_find.test.cpp")
add_src_file  (FILES_test_pcm_detail "ni/media/pcm/detail/tuple_to_array.test.cpp")
add_src_file  (FILES_test_pcm_detail "ni/media/pcm/detail/deep_type_replace.test.cpp")
add_src_group (FILES_test_all test_pcm_detail FILES_test_pcm_detail)

#--------------------------------------------------------------------
# linking
#--------------------------------------------------------------------

add_executable              ( pcm_test  ${FILES_test_all} )
target_include_directories  ( pcm_test PRIVATE . )
target_link_libraries       ( pcm_test PRIVATE pcm GTest::GTest)

if(MSVC)
  target_compile_options    ( pcm_test PRIVATE /wd4512 /bigobj)
endif()

ni_add_gtest( pcm_test )
