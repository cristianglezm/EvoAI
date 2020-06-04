#
# Try to find GMock library and include path.
# Once done this will define
#
# GMock_FOUND
# GMock_INCLUDE_DIR
# GMock_LIBRARIES
# GMock_ROOT
#

set(FIND_GMock_PATHS
    ${GMock_ROOT}
    $ENV{GMock_ROOT}
    /usr/local
    /usr/local/include
    /usr/lib
    /usr
    /sw
    /opt/local
    /opt/csw
    /opt)

if(GMock_ROOT)
	SET(GMock_INCLUDE_DIR "${GMock_ROOT}/include")
	SET(GMock_LIBRARIES "${GMock_ROOT}/lib/libgmock.a" "${GMock_ROOT}/lib/libgmock_main.a")
	SET(GMock_FOUND 1)
endif(GMock_ROOT)

find_path(GMock_INCLUDE_DIR gmock/gmock.h
          PATH_SUFFIXES include
          PATHS ${FIND_GMock_PATHS})

find_library(GMock_LIBRARY
		NAMES gmock
		PATH_SUFFIXES lib
		PATHS ${FIND_GMock_PATHS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(gmock DEFAULT_MSG GMock_LIBRARY GMock_INCLUDE_DIR)

set(GMock_INCLUDE_DIR "${GMock_INCLUDE_DIR}")
set(GMock_LIBRARIES "${GMock_LIBRARY}")

if(NOT GMock_FOUND)
	set(FIND_GMock_ERROR "Could NOT find GMock")
	if(GMock_FIND_REQUIRED)
		message(FATAL_ERROR ${FIND_GMock_ERROR})
	endif()
endif()
