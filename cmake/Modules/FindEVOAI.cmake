#
# Try to find EvoAI library and include path.
# Once done this will define
#
# EvoAI_FOUND
# EvoAI_INCLUDE_DIR
# EvoAI_LIBRARIES
# EvoAI_ROOT
#

set(FIND_EvoAI_PATHS
    ${EvoAI_ROOT}
    $ENV{EvoAI_ROOT}
    /usr/local
    /usr
    /sw
    /opt/local
    /opt/csw
    /opt)

if(EvoAI_ROOT)
	SET(EvoAI_INCLUDE_DIR "${EvoAI_ROOT}/include")
	SET(EvoAI_LIBRARIES "${EvoAI_ROOT}/lib/libEvoAI.a")
	SET(EvoAI_FOUND 1)
endif(EvoAI_ROOT)

find_path(EvoAI_INCLUDE_DIR EvoAI/include/EvoAI.hpp
          PATH_SUFFIXES include
          PATHS ${FIND_EvoAI_PATHS})

find_library(EvoAI_LIBRARY
		NAMES EvoAI
		PATH_SUFFIXES lib
		PATHS ${FIND_EvoAI_PATHS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(EvoAI DEFAULT_MSG EvoAI_LIBRARY EvoAI_INCLUDE_DIR)

# check the version number
set(EvoAI_VERSION_OK TRUE)
if(EvoAI_FIND_VERSION AND EvoAI_INCLUDE_DIR)
    set(EvoAI_CONFIG_HPP_INPUT "${EvoAI_INCLUDE_DIR}/EvoAI/Config.hpp")
    FILE(READ "${EvoAI_CONFIG_HPP_INPUT}" EvoAI_CONFIG_HPP_CONTENTS)
    STRING(REGEX REPLACE ".*#define EvoAI_VERSION_MAJOR ([0-9]+).*" "\\1" EvoAI_VERSION_MAJOR "${EvoAI_CONFIG_HPP_CONTENTS}")
    STRING(REGEX REPLACE ".*#define EvoAI_VERSION_MINOR ([0-9]+).*" "\\1" EvoAI_VERSION_MINOR "${EvoAI_CONFIG_HPP_CONTENTS}")
    STRING(REGEX REPLACE ".*#define EvoAI_VERSION_PATCH ([0-9]+).*" "\\1" EvoAI_VERSION_PATCH "${EvoAI_CONFIG_HPP_CONTENTS}")
    if (NOT "${EvoAI_VERSION_PATCH}" MATCHES "^[0-9]+$")
        set(EvoAI_VERSION_PATCH 0)
    endif()
    math(EXPR EvoAI_REQUESTED_VERSION "${EvoAI_FIND_VERSION_MAJOR} * 10000 + ${EvoAI_FIND_VERSION_MINOR} * 100 + ${EvoAI_FIND_VERSION_PATCH}")

    # if we could extract them, compare with the requested version number
    if (EvoAI_VERSION_MAJOR)
        # transform version numbers to an integer
        math(EXPR EvoAI_VERSION "${EvoAI_VERSION_MAJOR} * 10000 + ${EvoAI_VERSION_MINOR} * 100 + ${EvoAI_VERSION_PATCH}")

        # compare them
        if(EvoAI_VERSION LESS EvoAI_REQUESTED_VERSION)
            set(EvoAI_VERSION_OK FALSE)
        endif()
    else()
        # EvoAI version is < 2.0
        if(EvoAI_REQUESTED_VERSION GREATER 10900)
            set(EvoAI_VERSION_OK FALSE)
            set(EvoAI_VERSION_MAJOR 1)
            set(EvoAI_VERSION_MINOR x)
            set(EvoAI_VERSION_PATCH x)
        endif()
    endif()
endif()

set(EvoAI_INCLUDE_DIR "${EvoAI_INCLUDE_DIR}")
set(EvoAI_LIBRARIES "${EvoAI_LIBRARY}")


if(NOT EvoAI_VERSION_OK)
	set(FIND_EvoAI_ERROR "EvoAI found but version too low (requested: ${EvoAI_FIND_VERSION}}, found: ${EvoAI_VERSION_MAJOR}.${EvoAI_VERSION_MINOR}.${EvoAI_VERSION_PATCH})")
	set(EvoAI_FOUND FALSE)
elseif(NOT EvoAI_FOUND)
	set(FIND_EvoAI_ERROR "Could NOT find EvoAI")
	if(EvoAI_FIND_REQUIRED)
		message(FATAL_ERROR ${FIND_EvoAI_ERROR})
	endif()
endif()

if(NOT EvoAI_FOUND)
	set(FIND_EvoAI_ERROR "Could NOT find EvoAI")
	if(EvoAI_FIND_REQUIRED)
		message(FATAL_ERROR ${FIND_EvoAI_ERROR})
	endif()
endif()
