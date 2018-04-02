function( ni_treat_warnings_as_errors Target )

  if( CMAKE_GENERATOR MATCHES "Xcode" )
    set_target_properties( ${Target} PROPERTIES XCODE_ATTRIBUTE_GCC_TREAT_WARNINGS_AS_ERRORS "YES" )
    target_compile_options( ${Target} PRIVATE -Wno-deprecated-declarations)
  elseif( CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU" )
    target_compile_options( ${Target} PRIVATE -Werror -Wno-deprecated-declarations )
  elseif( MSVC )
    target_compile_options( ${Target} PRIVATE /WX /wd4996 )
  endif()
 
endfunction()