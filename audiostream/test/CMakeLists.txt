
set( SCOPED_DISABLE_UNITY_BUILDS 1 )

add_src_file  ( FILES_main "gtest_main.cpp" )
add_src_group ( FILES_test_all test_main FILES_main )


#--------------------------------------------------------------------
#

add_src_file  (FILES_test "ni/media/test_helper.cpp" WITH_HEADER)
add_src_file  (FILES_test "ni/media/generators.h")
add_src_file  (FILES_test "ni/media/statistics.h")
add_src_file  (FILES_test "ni/media/read_interlaced_test.h")
add_src_file  (FILES_test "ni/media/reference_test.h")
add_src_file  (FILES_test "ni/media/sink_test.h" )
add_src_file  (FILES_test "ni/media/source_test.cpp" WITH_HEADER)

add_src_group (FILES_test_all test FILES_test )

#--------------------------------------------------------------------
# generators

add_src_file  (FILES_test_generators "ni/media/generators/modulated_sine.h")
add_src_group (FILES_test_all test_generators FILES_test_generators)


#--------------------------------------------------------------------
# iostreams

add_src_file  (FILES_test_iostreams "ni/media/iostreams/stream_buffer.test.cpp")
add_src_group (FILES_test_all test_iostreams FILES_test_iostreams)

#--------------------------------------------------------------------
# audio

add_src_file  (FILES_test_audio "ni/media/audio/istream_read.test.h")
add_src_file  (FILES_test_audio "ni/media/audio/istream_read_signed_integer.test.cpp" )
add_src_file  (FILES_test_audio "ni/media/audio/istream_read_unsigned_integer.test.cpp")
add_src_file  (FILES_test_audio "ni/media/audio/istream_read_floating_point.test.cpp"  )
add_src_file  (FILES_test_audio "ni/media/audio/ifstream.test.cpp" )
add_src_file  (FILES_test_audio "ni/media/audio/ifstream_robustness.test.cpp")
add_src_file  (FILES_test_audio "ni/media/audio/ifvectorstream.test.cpp" )
add_src_file  (FILES_test_audio "ni/media/audio/ivectorstream.test.cpp" )
add_src_file  (FILES_test_audio "ni/media/audio/ofstream.test.cpp" )
add_src_group (FILES_test_all test_audio FILES_test_audio)

#--------------------------------------------------------------------
# audio/aiff

if( NIMEDIA_ENABLE_AIFF_DECODING )
  add_src_file  (FILES_test_audio_aiff "ni/media/audio/aiff/aiff_ifstream_info.test.cpp")
  add_src_file  (FILES_test_audio_aiff "ni/media/audio/aiff/aiff_source.test.cpp")
endif()

if( NIMEDIA_ENABLE_AIFF_ENCODING )
  add_src_file  (FILES_test_audio_wav "ni/media/audio/aiff/aiff_sink.test.cpp")
endif()

if( FILES_test_audio_aiff )
  add_src_group (FILES_test_all test_audio_aiff FILES_test_audio_aiff)
endif()

#--------------------------------------------------------------------
# audio/flac

if( NIMEDIA_ENABLE_FLAC_DECODING )
  add_src_file  (FILES_test_audio_flac "ni/media/audio/flac/flac_source.test.cpp")
  add_src_group (FILES_test_all test_audio_flac FILES_test_audio_flac)
endif()

#--------------------------------------------------------------------
# audio/mp3

if( NIMEDIA_ENABLE_MP3_DECODING )
  add_src_file  (FILES_test_audio_mp3 "ni/media/audio/mp3/mp3_source.test.cpp" MAC)
  add_src_group (FILES_test_all test_audio_mp3 FILES_test_audio_mp3)
endif()

#--------------------------------------------------------------------
# audio/mp4

if( NIMEDIA_ENABLE_MP4_DECODING )
  add_src_file  (FILES_test_audio_mp4 "ni/media/audio/mp4/mp4_source.test.cpp" MAC)
  add_src_group (FILES_test_all test_audio_mp4 FILES_test_audio_mp4)
endif()

#--------------------------------------------------------------------
# audio/ogg

if( NIMEDIA_ENABLE_OGG_DECODING )
  add_src_file  (FILES_test_audio_ogg "ni/media/audio/ogg/ogg_source.test.cpp")
  add_src_group (FILES_test_all test_audio_ogg FILES_test_audio_ogg)
endif()

#--------------------------------------------------------------------
# audio/wav

if( NIMEDIA_ENABLE_WAV_DECODING )
  add_src_file  (FILES_test_audio_wav "ni/media/audio/wav/wav_ifstream_info.test.cpp")
  add_src_file  (FILES_test_audio_wav "ni/media/audio/wav/wav_source.test.cpp")
endif()

if( NIMEDIA_ENABLE_WAV_ENCODING )
  add_src_file  (FILES_test_audio_wav "ni/media/audio/wav/wav_ofstream_info.test.cpp")
  add_src_file  (FILES_test_audio_wav "ni/media/audio/wav/wav_sink.test.cpp")
endif()

if( FILES_test_audio_wav )
  add_src_group (FILES_test_all test_audio_wav FILES_test_audio_wav)
endif()

#--------------------------------------------------------------------
# audio/wma

if( NIMEDIA_ENABLE_WMA_DECODING )
  add_src_file  (FILES_test_audio_wma "ni/media/audio/wma/wma_source.test.cpp")
  add_src_group (FILES_test_all test_audio_wma FILES_test_audio_wma)
endif()


#--------------------------------------------------------------------
# linking
#--------------------------------------------------------------------

add_executable              ( audiostream_test  ${FILES_test_all} )
target_include_directories  ( audiostream_test PRIVATE . ../src )
target_link_libraries       ( audiostream_test PRIVATE audiostream GTest::GTest)

if(MSVC)
  target_compile_options      ( audiostream_test PRIVATE /wd4512 /bigobj)
endif()

ni_add_test( audiostream_test GTEST )

set( test_files_path ${CMAKE_CURRENT_SOURCE_DIR}/test_files)
set( output_files_path ${CMAKE_CURRENT_BINARY_DIR}/test_files)

target_compile_definitions  ( audiostream_test PRIVATE "NI_MEDIA_TEST_FILES_PATH=${test_files_path}" )
target_compile_definitions  ( audiostream_test PRIVATE "NI_MEDIA_OUTPUT_FILES_PATH=${output_files_path}" )

file(MAKE_DIRECTORY ${output_files_path})

#--------------------------------------------------------------------
# reference generator
#--------------------------------------------------------------------

if(NIMEDIA_ENABLE_WAV_ENCODING)

    add_src_file  (FILES_generator "generator.cpp"    )
    add_src_group (FILES_generator generator FILES_generator)

    add_executable        ( generator ${FILES_generator} )
    target_include_directories  ( generator PRIVATE . )
    target_link_libraries ( generator PRIVATE audiostream Boost::program_options )

endif()



