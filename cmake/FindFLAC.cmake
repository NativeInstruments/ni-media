include(FindPackageHandleStandardArgs)

find_package(Ogg REQUIRED)

find_path(FLAC_INCLUDE_DIRS FLAC/stream_decoder.h)
find_library(FLAC_LIBRARIES NAMES FLAC)
find_path(FLACPP_INCLUDE_DIRS FLAC++/decoder.h)
find_library(FLACPP_LIBRARIES NAMES FLAC++)

find_package_handle_standard_args(FLAC DEFAULT_MSG FLAC_INCLUDE_DIRS FLAC_LIBRARIES FLACPP_INCLUDE_DIRS FLACPP_LIBRARIES)
mark_as_advanced(FLAC_INCLUDE_DIRS FLAC_LIBRARIES FLACPP_INCLUDE_DIRS FLACPP_LIBRARIES)

if( FLAC_FOUND ) 
  if(  NOT TARGET FLAC::flac )
    add_library(FLAC::flac UNKNOWN IMPORTED)
    set_target_properties(FLAC::flac PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "C"
    IMPORTED_LOCATION ${FLAC_LIBRARIES}
    INTERFACE_INCLUDE_DIRECTORIES "${FLAC_INCLUDE_DIRS}"
	  )

    target_link_libraries(FLAC::flac INTERFACE Ogg::ogg)

  endif()

  # TODO: make flacpp an optional component

  if( NOT TARGET FLAC::flacpp )
    add_library(FLAC::flacpp UNKNOWN IMPORTED)
    set_target_properties(FLAC::flacpp PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
      IMPORTED_LOCATION ${FLACPP_LIBRARIES}
      INTERFACE_INCLUDE_DIRECTORIES "${FLACPP_INCLUDE_DIRS}"
  	)

    target_link_libraries(FLAC::flacpp INTERFACE FLAC::flac)
  endif()
endif()

