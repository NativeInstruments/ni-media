find_package (PkgConfig REQUIRED)
pkg_check_modules (GOBJECT2 REQUIRED gobject-2.0)

include_directories (${GOBJECT2_INCLUDE_DIRS})
link_directories (${GOBJECT2_LIBRARY_DIRS})
