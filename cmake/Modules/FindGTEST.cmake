#
# Try to find GTEST library and include path.
# Once done this will define
#
# GTEST_FOUND
# GTEST_INCLUDE_DIR
# GTEST_LIBRARIES
# GTEST_ROOT
#

set(FIND_GTEST_PATHS
    ${GTEST_ROOT}
    $ENV{GTEST_ROOT}
    /usr/local
    /usr
    /sw
    /opt/local
    /opt/csw
    /opt)

if(GTEST_ROOT)
	SET(GTEST_INCLUDE_DIR "${GTEST_ROOT}/include")
	SET(GTEST_LIBRARIES "${GTEST_ROOT}/lib/libgtest.a" "${GTEST_ROOT}/lib/libgtest_main.a")
	SET(GTEST_FOUND 1)
endif(GTEST_ROOT)

find_path(GTEST_INCLUDE_DIR gtest/gtest.h
          PATH_SUFFIXES include
          PATHS ${FIND_GTEST_PATHS})

find_library(GTEST_LIBRARY
		NAMES GTEST
		PATH_SUFFIXES lib
		PATHS ${FIND_GTEST_PATHS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GTEST DEFAULT_MSG GTEST_LIBRARY GTEST_INCLUDE_DIR)

set(GTEST_INCLUDE_DIR "${GTEST_INCLUDE_DIR}")
set(GTEST_LIBRARIES "${GTEST_LIBRARY}")

if(NOT GTEST_FOUND)
	set(FIND_GTEST_ERROR "Could NOT find GTEST")
	if(GTEST_FIND_REQUIRED)
		message(FATAL_ERROR ${FIND_GTEST_ERROR})
	endif()
endif()
