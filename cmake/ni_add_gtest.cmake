
function(ni_add_gtest Target)
  if( ARGV1 )
    set( GtestFilter "${ARGV1}")
  else()
    set( GtestFilter "-*.*_NOBS*")
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
    COMMAND $<TARGET_FILE:${Target}> --gtest_output=xml:test-reports/${TestOutput} --gtest_filter=${GtestFilter}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

endfunction()