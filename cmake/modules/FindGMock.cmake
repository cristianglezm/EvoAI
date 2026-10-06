#
# Try to find GMock library and include path.
# Once done this will define
#
# GMock_FOUND
# GMock_INCLUDE_DIR
# GMock_LIBRARIES
# GMock_LIBRARY
# GMock_MAIN_LIBRARY
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
    /opt
)

# Search the explicitly specified root first.
if(GMock_ROOT)
    find_path(GMock_INCLUDE_DIR
        gmock/gmock.h
        PATH_SUFFIXES include
        PATHS "${GMock_ROOT}"
        NO_CMAKE_FIND_ROOT_PATH
        NO_DEFAULT_PATH
    )

    find_library(GMock_LIBRARY
        NAMES gmock gmockd GMock
        PATH_SUFFIXES lib
        PATHS "${GMock_ROOT}"
        NO_CMAKE_FIND_ROOT_PATH
        NO_DEFAULT_PATH
    )

    find_library(GMock_MAIN_LIBRARY
        NAMES gmock_main gmock_maind
        PATH_SUFFIXES lib
        PATHS "${GMock_ROOT}"
        NO_CMAKE_FIND_ROOT_PATH
        NO_DEFAULT_PATH
    )
endif()

# Fall back to normal CMake search paths if necessary.
if(NOT GMock_INCLUDE_DIR)
    find_path(GMock_INCLUDE_DIR
        gmock/gmock.h
        PATH_SUFFIXES include
        PATHS ${FIND_GMock_PATHS}
    )
endif()

if(NOT GMock_LIBRARY)
    find_library(GMock_LIBRARY
        NAMES gmock gmockd GMock
        PATH_SUFFIXES lib
        PATHS ${FIND_GMock_PATHS}
    )
endif()

if(NOT GMock_MAIN_LIBRARY)
    find_library(GMock_MAIN_LIBRARY
        NAMES gmock_main gmock_maind
        PATH_SUFFIXES lib
        PATHS ${FIND_GMock_PATHS}
    )
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
    GMock
    DEFAULT_MSG
    GMock_INCLUDE_DIR
    GMock_LIBRARY
)

if(GMock_FOUND)
    set(GMock_LIBRARIES "${GMock_LIBRARY}")

    if(GMock_MAIN_LIBRARY)
        list(APPEND GMock_LIBRARIES "${GMock_MAIN_LIBRARY}")
    endif()
else()
    set(FIND_GMock_ERROR "Could NOT find GMock")
    if(GMock_FIND_REQUIRED)
        message(FATAL_ERROR "${FIND_GMock_ERROR}")
    endif()
endif()
