include(FindPackageHandleStandardArgs)

find_package(Ogg REQUIRED)
find_path(Vorbis_INCLUDE_DIRS vorbis/vorbisfile.h)
find_library(Vorbis_LIBRARIES NAMES vorbis)


find_package_handle_standard_args(Vorbis DEFAULT_MSG Vorbis_INCLUDE_DIRS Vorbis_LIBRARIES)
mark_as_advanced(Vorbis_INCLUDE_DIRS Vorbis_LIBRARIES )

if( Vorbis_FOUND AND NOT TARGET vorbis )
  add_library(vorbis UNKNOWN IMPORTED)
  set_target_properties(vorbis PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "C"
    IMPORTED_LOCATION ${Vorbis_LIBRARIES}
    INTERFACE_INCLUDE_DIRECTORIES "${Vorbis_INCLUDE_DIRS};${Ogg_INCLUDE_DIRS}"
    INTERFACE_LINK_LIBRARIES "${Ogg_LIBRARIES}"
	)
endif()


find_library(Vorbisfile_LIBRARIES NAMES vorbisfile)
find_package_handle_standard_args(Vorbis DEFAULT_MSG Vorbisfile_LIBRARIES)
mark_as_advanced(Vorbisfile_LIBRARIES)

if( Vorbisfile_FOUND AND NOT TARGET vorbisfile )
  add_library(vorbisfile UNKNOWN IMPORTED)
  set_target_properties(vorbisfile PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "C"
    IMPORTED_LOCATION ${Vorbisfile_LIBRARIES}
    INTERFACE_INCLUDE_DIRECTORIES "${Vorbis_INCLUDE_DIRS};${Ogg_INCLUDE_DIRS}"
    INTERFACE_LINK_LIBRARIES "${Vorbisfile_LIBRARIES};${Vorbis_LIBRARIES};${Ogg_LIBRARIES}"
	)
endif()


find_library(Vorbisenc_LIBRARIES NAMES vorbisenc)
find_package_handle_standard_args(Vorbis DEFAULT_MSG Vorbisenc_LIBRARIES)
mark_as_advanced(Vorbisenc_LIBRARIES)

if( Vorbisenc_FOUND AND NOT TARGET vorbisenc )
  add_library(vorbisenc UNKNOWN IMPORTED)
  set_target_properties(vorbisenc PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "C"
    IMPORTED_LOCATION ${Vorbisenc_LIBRARIES}
    INTERFACE_INCLUDE_DIRECTORIES "${Vorbis_INCLUDE_DIRS};${Ogg_INCLUDE_DIRS}"
    INTERFACE_LINK_LIBRARIES "${Vorbisenc_LIBRARIES};${Vorbis_LIBRARIES};${Ogg_LIBRARIES}"
	)
endif()

