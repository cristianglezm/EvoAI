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

find_path(EvoAI_INCLUDE_DIR EvoAI/include/EvoAI.h
          PATH_SUFFIXES include
          PATHS ${FIND_EvoAI_PATHS})

find_library(EvoAI_LIBRARY
		NAMES EvoAI
		PATH_SUFFIXES lib
		PATHS ${FIND_EvoAI_PATHS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(EvoAI DEFAULT_MSG EvoAI_LIBRARY EvoAI_INCLUDE_DIR)

set(EvoAI_INCLUDE_DIR "${EvoAI_INCLUDE_DIR}")
set(EvoAI_LIBRARIES "${EvoAI_LIBRARY}")

if(NOT EvoAI_FOUND)
	set(FIND_EvoAI_ERROR "Could NOT find EvoAI")
	if(EvoAI_FIND_REQUIRED)
		message(FATAL_ERROR ${FIND_EvoAI_ERROR})
	endif()
endif()
