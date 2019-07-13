include(FindPackageHandleStandardArgs)

find_package(Ogg REQUIRED)

find_path(Vorbis_INCLUDE_DIRS vorbis/vorbisfile.h)
find_library(Vorbis_LIBRARIES NAMES vorbis)
find_library(Vorbisfile_LIBRARIES NAMES vorbisfile)
find_library(Vorbisenc_LIBRARIES NAMES vorbisenc)

find_package_handle_standard_args(Vorbis DEFAULT_MSG Vorbis_INCLUDE_DIRS Vorbis_LIBRARIES Vorbisfile_LIBRARIES Vorbisenc_LIBRARIES)
mark_as_advanced(Vorbis_INCLUDE_DIRS Vorbis_LIBRARIES Vorbisfile_LIBRARIES Vorbisenc_LIBRARIES)

if( VORBIS_FOUND )
  if( NOT TARGET Vorbis::vorbis )
    add_library(Vorbis::vorbis UNKNOWN IMPORTED)
    set_target_properties(Vorbis::vorbis PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES "C"
      IMPORTED_LOCATION ${Vorbis_LIBRARIES}
      INTERFACE_INCLUDE_DIRECTORIES "${Vorbis_INCLUDE_DIRS}"
    )

    target_link_libraries(Vorbis::vorbis INTERFACE Ogg::ogg)
  endif()

  # TODO: make vorbisfile and vorbisenc optional components

  if( NOT TARGET Vorbis::vorbisfile )
    add_library(Vorbis::vorbisfile UNKNOWN IMPORTED)
    set_target_properties(Vorbis::vorbisfile PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES "C"
      IMPORTED_LOCATION ${Vorbisfile_LIBRARIES}
    )

    target_link_libraries(Vorbis::vorbisfile INTERFACE Vorbis::vorbis)

  endif()


  if( NOT TARGET Vorbis::vorbisenc )
    add_library(Vorbis::vorbisenc UNKNOWN IMPORTED)
    set_target_properties(Vorbis::vorbisenc PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES "C"
      IMPORTED_LOCATION ${Vorbisenc_LIBRARIES}
    )

    target_link_libraries(Vorbis::vorbisenc INTERFACE Vorbis::vorbis)

  endif()
endif()



