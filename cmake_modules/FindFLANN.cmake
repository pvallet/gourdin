###############################################################################
# Find Flann
#
# This sets the following variables:
# FLANN_FOUND - True if FLANN was found.
# FLANN_INCLUDE_DIRS - Directories containing the FLANN include files.
# FLANN_LIBRARIES - Libraries needed to use FLANN.
# FLANN_DEFINITIONS - Compiler flags for FLANN.


# On windows, we need to use a version that was built locally
find_path(FLANN_INCLUDE_DIR flann/flann.hpp
PATH ${FLANN_LOCATION}/src/cpp)

find_library(FLANN_LIBRARY flann_cpp
PATH ${FLANN_LOCATION}/lib)

# On linux, if we don't find the local version, look it on the system
if (${CMAKE_HOST_UNIX})
  find_package(PkgConfig)
  pkg_check_modules(PC_FLANN flann)

  set(FLANN_DEFINITIONS ${PC_FLANN_CFLAGS_OTHER})

  find_path(FLANN_INCLUDE_DIR flann/flann.hpp
    HINTS ${PC_FLANN_INCLUDEDIR} ${PC_FLANN_INCLUDE_DIRS})

  find_library(FLANN_LIBRARY flann
    HINTS ${PC_FLANN_LIBDIR} ${PC_FLANN_LIBRARY_DIRS})

endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Flann DEFAULT_MSG
    FLANN_LIBRARY FLANN_INCLUDE_DIR)

if (FLANN_FOUND)
  set(FLANN_INCLUDE_DIRS ${FLANN_INCLUDE_DIR})
  set(FLANN_LIBRARIES ${FLANN_LIBRARY})
endif()

mark_as_advanced(FLANN_LIBRARY FLANN_INCLUDE_DIR)
