include(FindPackageHandleStandardArgs)

find_path(Ogg_INCLUDE_DIRS ogg/ogg.h)
find_library(Ogg_LIBRARIES NAMES ogg)

find_package_handle_standard_args(Ogg DEFAULT_MSG Ogg_INCLUDE_DIRS Ogg_LIBRARIES )
mark_as_advanced(Ogg_INCLUDE_DIRS Ogg_LIBRARIES )

if( OGG_FOUND )
  if( NOT TARGET Ogg::ogg )
    add_library(Ogg::ogg UNKNOWN IMPORTED)
    set_target_properties(Ogg::ogg PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES "C"
      IMPORTED_LOCATION ${Ogg_LIBRARIES}
      INTERFACE_INCLUDE_DIRECTORIES ${Ogg_INCLUDE_DIRS}
    )
  endif()
endif()


