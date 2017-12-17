
################################################
# Workaround for older cmake versions. can be removed for cmake > 3.5

macro(ni_find_gtest)

  find_package(GTest REQUIRED)
  # Workaroud: older cmake versions do not create an imported target for GTest
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

      if( LINUX )

          set( THREADS_PREFER_PTHREAD_FLAG ON )
          find_package( Threads REQUIRED )

          if(THREADS_FOUND AND NOT TARGET Threads::Threads)
            add_library(Threads::Threads INTERFACE IMPORTED)

            if(THREADS_HAVE_PTHREAD_ARG)
              set_property(TARGET Threads::Threads PROPERTY INTERFACE_COMPILE_OPTIONS "-pthread")
            endif()

            if(CMAKE_THREAD_LIBS_INIT)
              set_property(TARGET Threads::Threads PROPERTY INTERFACE_LINK_LIBRARIES "${CMAKE_THREAD_LIBS_INIT}")
            endif()
          endif()

      endif()

  endif()

  set_property (TARGET GTest::GTest APPEND PROPERTY
        INTERFACE_COMPILE_DEFINITIONS "-DGTEST_LANG_CXX11=1"
    )

endmacro()
