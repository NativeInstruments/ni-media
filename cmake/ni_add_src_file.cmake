#################################################################################################################
# generic options:
# WITH_HEADER             header of the same name next to source file
# WITH_PUBLIC_HEADER      header with then same name in inc folder
# OPTIMIZE_DEBUG          optimize this source file when building in release mode
# DONT_COMPILE            add source file to project, but do not compile it
#
# platform options:
# WIN
# MAC
# IOS
# ANDROID
# LINUX
# APPLE                   IOS or MAC
# UNIX                    APPLE or LINUX
# DESKTOP                 WIN or MAC
# MOBILE                  IOS or ANDROID

function(add_src_file COLLECTION FILE)
  list(APPEND ${COLLECTION} ${FILE})

  include(CMakeParseArguments)

  # handle arguments
  set( Options WITH_HEADER WITH_PUBLIC_HEADER OPTIMIZE_DEBUG DONT_COMPILE
               WIN MAC IOS ANDROID LINUX APPLE DESKTOP MOBILE UNIX
     )

  cmake_parse_arguments( Option "${Options}" "" "" ${ARGN} )

  # handle platform tags
  if( Option_WIN OR Option_MAC OR Option_IOS OR Option_ANDROID OR Option_LINUX OR
      Option_APPLE OR Option_DESKTOP OR Option_MOBILE )

    if( APPLE AND NOT IOS ) # macos

      if( NOT (Option_APPLE OR Option_MAC OR Option_DESKTOP OR Option_UNIX) )
        set( EXCLUDE_ON_CURRENT_PLATFORM true )
      endif()

    elseif( IOS )

      if( NOT (Option_APPLE OR Option_IOS OR Option_MOBILE) )
        set( EXCLUDE_ON_CURRENT_PLATFORM true )
      endif()

    elseif( WIN32 )

      if( NOT (Option_WIN OR Option_DESKTOP) )
        set( EXCLUDE_ON_CURRENT_PLATFORM true )
      endif()

    elseif( LINUX )

      if( NOT (Option_LINUX OR Option_DESKTOP OR Option_UNIX) )
        set( EXCLUDE_ON_CURRENT_PLATFORM true )
      endif()
    else()
      message( FATAL_ERROR "support for current platform not implemented" )
    endif()
  endif()
  # platform handling end


  if( Option_DONT_COMPILE OR EXCLUDE_ON_CURRENT_PLATFORM )
    set_source_files_properties(${FILE} PROPERTIES HEADER_FILE_ONLY true)
  endif()

  if( Option_WITH_HEADER )
    string(REGEX REPLACE ".([^.]+)$" ".h" header ${FILE})
    list(APPEND ${COLLECTION} ${header})
  endif()

  if( Option_WITH_PUBLIC_HEADER )
    string(REGEX REPLACE ".([^.]+)$" ".h" header ${FILE})
    string(REGEX REPLACE "src/" "inc/" header ${header})
    list(APPEND ${COLLECTION} ${header})
  endif()

  if( Option_OPTIMIZE_DEBUG )

    if( NOT (CMAKE_BUILD_TYPE STREQUAL "RELEASE" OR CMAKE_BUILD_TYPE STREQUAL "RELWITHDEBINFO") )
      set_source_files_properties(${FILE} PROPERTIES HEADER_FILE_ONLY false)
    endif()

    if(MSVC)
      set_source_files_properties(${FILE} PROPERTIES COMPILE_FLAGS "/Ob2 /O2")
    elseif( CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU" )
      set_source_files_properties(${FILE} PROPERTIES COMPILE_FLAGS "-O2")
    endif()
  endif()

  set(${COLLECTION} ${${COLLECTION}} PARENT_SCOPE)
endfunction()

