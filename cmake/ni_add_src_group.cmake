
################################################
# aggregate sources and create project structure

function(add_src_group ALL_FILES COMPILE_GROUP_NAME SRC_FILES)

  if (COMPILE_GROUP_NAME MATCHES "\\\\")
    message(WARNING "Naming deprecated: ${COMPILE_GROUP_NAME}")
  endif()

  string(REGEX REPLACE "\\\\" "_" CH_NAME "${COMPILE_GROUP_NAME}")

  foreach(file ${${SRC_FILES}})
    get_filename_component(dir ${file} DIRECTORY)
    if(dir)
      string(REPLACE / \\ dir ${dir})
      source_group("${dir}" FILES ${file})
    endif()
  endforeach()

  set(${SRC_FILES} ${${SRC_FILES}} PARENT_SCOPE)
  set(${ALL_FILES} "${${ALL_FILES}};${${SRC_FILES}}" PARENT_SCOPE)
endfunction()
