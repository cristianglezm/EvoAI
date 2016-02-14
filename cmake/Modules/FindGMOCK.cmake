#
# Try to find GMOCK library and include path.
# Once done this will define
#
# GMOCK_FOUND
# GMOCK_INCLUDE_DIR
# GMOCK_LIBRARIES
# GMOCK_ROOT
#

set(FIND_GMOCK_PATHS
    ${GMOCK_ROOT}
    $ENV{GMOCK_ROOT}
    /usr/local
    /usr
    /sw
    /opt/local
    /opt/csw
    /opt)

if(GMOCK_ROOT)
	SET(GMOCK_INCLUDE_DIR "${GMOCK_ROOT}/include")
	SET(GMOCK_LIBRARIES "${GMOCK_ROOT}/lib/libgmock.a" "${GMOCK_ROOT}/lib/libgmock_main.a")
	SET(GMOCK_FOUND 1)
endif(GMOCK_ROOT)

find_path(GMOCK_INCLUDE_DIR gmock/gmock.h
          PATH_SUFFIXES include
          PATHS ${FIND_GMOCK_PATHS})

find_library(GMOCK_LIBRARY
		NAMES GMOCK
		PATH_SUFFIXES lib
		PATHS ${FIND_GMOCK_PATHS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GMOCK DEFAULT_MSG GMOCK_LIBRARY GMOCK_INCLUDE_DIR)

set(GMOCK_INCLUDE_DIR "${GMOCK_INCLUDE_DIR}")
set(GMOCK_LIBRARIES "${GMOCK_LIBRARY}")

if(NOT GMOCK_FOUND)
	set(FIND_GMOCK_ERROR "Could NOT find GMOCK")
	if(GMOCK_FIND_REQUIRED)
		message(FATAL_ERROR ${FIND_GMOCK_ERROR})
	endif()
endif()
