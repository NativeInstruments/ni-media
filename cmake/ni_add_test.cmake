
function(ni_add_test Target)

  cmake_parse_arguments( PARSE_ARGV 1 NI_ADD_TEST "GTEST" "" "" )

  if( NOT NI_ADD_TEST_GTEST )
    message( SEND_ERROR "ni_add_test: GTEST must be specified" )
  endif()

  if( UNIX )
    set( TestOutput ${Target}.xml)
  elseif(WIN64)
    set( TestOutput ${Target}64.xml)
  elseif(WIN32)
    set( TestOutput ${Target}32.xml)
  else()
    message(WARNING "Unsupported Platform")
    set( TestOutput ${Target}.xml)
  endif()

  add_test(NAME ${Target}_run
    COMMAND $<TARGET_FILE:${Target}> --gtest_output=xml:test-reports/${TestOutput}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

endfunction()