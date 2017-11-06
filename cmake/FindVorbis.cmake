include(FindPackageHandleStandardArgs)

find_package(Ogg REQUIRED)
find_path(Vorbis_INCLUDE_DIRS vorbis/vorbisfile.h)
find_library(Vorbis_LIBRARIES NAMES vorbis)



if( Vorbis_LIBRARIES AND NOT TARGET vorbis )
  add_library(vorbis UNKNOWN IMPORTED)
  set_target_properties(vorbis PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "C"
    IMPORTED_LOCATION ${Vorbis_LIBRARIES}
    INTERFACE_INCLUDE_DIRECTORIES "${Vorbis_INCLUDE_DIRS};${Ogg_INCLUDE_DIRS}"
    INTERFACE_LINK_LIBRARIES "${Ogg_LIBRARIES}"
	)
endif()


find_library(Vorbisfile_LIBRARIES NAMES vorbisfile)


if( Vorbisfile_LIBRARIES AND NOT TARGET vorbisfile )
  add_library(vorbisfile UNKNOWN IMPORTED)
  set_target_properties(vorbisfile PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "C"
    IMPORTED_LOCATION ${Vorbisfile_LIBRARIES}
    INTERFACE_INCLUDE_DIRECTORIES "${Vorbis_INCLUDE_DIRS};${Ogg_INCLUDE_DIRS}"
    INTERFACE_LINK_LIBRARIES "${Vorbisfile_LIBRARIES};${Vorbis_LIBRARIES};${Ogg_LIBRARIES}"
	)
endif()


find_library(Vorbisenc_LIBRARIES NAMES vorbisenc)

if( Vorbisenc_LIBRARIES AND NOT TARGET vorbisenc )
  add_library(vorbisenc UNKNOWN IMPORTED)
  set_target_properties(vorbisenc PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "C"
    IMPORTED_LOCATION ${Vorbisenc_LIBRARIES}
    INTERFACE_INCLUDE_DIRECTORIES "${Vorbis_INCLUDE_DIRS};${Ogg_INCLUDE_DIRS}"
    INTERFACE_LINK_LIBRARIES "${Vorbisenc_LIBRARIES};${Vorbis_LIBRARIES};${Ogg_LIBRARIES}"
	)
endif()

find_package_handle_standard_args(Vorbis DEFAULT_MSG Vorbis_INCLUDE_DIRS Vorbis_LIBRARIES Vorbisfile_LIBRARIES Vorbisenc_LIBRARIES)
mark_as_advanced(Vorbis_INCLUDE_DIRS Vorbis_LIBRARIES Vorbisfile_LIBRARIES Vorbisenc_LIBRARIES)