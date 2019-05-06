include(FindPackageHandleStandardArgs)

find_package(PkgConfig REQUIRED)

pkg_check_modules (GSTREAMER REQUIRED gstreamer-1.0)
find_library(GSTREAMER_LIBRARY NAMES gstreamer-1.0)

find_package_handle_standard_args(GSTREAMER DEFAULT_MSG GSTREAMER_INCLUDE_DIRS GSTREAMER_LIBRARY)
mark_as_advanced(GSTREAMER_INCLUDE_DIRS GSTREAMER_LIBRARY)


if( GSTREAMER_FOUND )
  if( NOT TARGET GSTREAMER::gstreamer )
    add_library(GSTREAMER::gstreamer SHARED IMPORTED)
    set_target_properties(GSTREAMER::gstreamer PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES "C"
      IMPORTED_LOCATION ${GSTREAMER_LIBRARY}
      INTERFACE_INCLUDE_DIRECTORIES "${GSTREAMER_INCLUDE_DIRS}"
      INTERFACE_LINK_LIBRARIES "${GSTREAMER_LIBRARY}"
    )
  endif()
endif()

