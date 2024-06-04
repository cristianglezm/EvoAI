#
# Try to find GTest library and include path.
# Once done this will define
#
# GTest_FOUND
# GTest_INCLUDE_DIR
# GTest_LIBRARIES
# GTest_LIBRARY
# GTest_ROOT
#

set(FIND_GTest_PATHS
    ${GTest_ROOT}
    $ENV{GTest_ROOT}
    /usr/local
    /usr
    /sw
    /opt/local
    /opt/csw
    /opt)

if(GTest_ROOT)
    if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	SET(GTest_INCLUDE_DIR "${GTest_ROOT}/include")
	SET(GTest_LIBRARIES "${GTest_ROOT}/lib/libgtest.lib" "${GTest_ROOT}/lib/libgtest_main.lib")
        SET(GTest_LIBRARY "${GTest_ROOT}/lib/libgtest.lib")
	SET(GTest_FOUND 1)
    else()
	SET(GTest_INCLUDE_DIR "${GTest_ROOT}/include")
	SET(GTest_LIBRARIES "${GTest_ROOT}/lib/libgtest.a" "${GTest_ROOT}/lib/libgtest_main.a")
        SET(GTest_LIBRARY "${GTest_ROOT}/lib/libgtest.a")
	SET(GTest_FOUND 1)
    endif()
endif(GTest_ROOT)

find_path(GTest_INCLUDE_DIR gtest/gtest.h
          PATH_SUFFIXES include
          PATHS ${FIND_GTest_PATHS})

find_library(GTest_LIBRARY
		NAMES GTest gtest.a gtest_main.a gtest.lib gtest_main.lib
		PATH_SUFFIXES lib
		PATHS ${FIND_GTest_PATHS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GTest DEFAULT_MSG GTest_LIBRARY GTest_INCLUDE_DIR)

set(GTest_INCLUDE_DIR "${GTest_INCLUDE_DIR}")
set(GTest_LIBRARIES "${GTest_LIBRARIES}" "${GTest_LIBRARY}")

if(NOT GTest_FOUND)
	set(FIND_GTest_ERROR "Could NOT find GTest")
	if(GTEST_FIND_REQUIRED)
		message(FATAL_ERROR ${FIND_GTEST_ERROR})
	endif()
endif()
