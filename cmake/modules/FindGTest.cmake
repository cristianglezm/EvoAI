#
# Try to find GTest library and include path.
# Once done this will define
#
# GTest_FOUND
# GTest_INCLUDE_DIR
# GTest_LIBRARIES
# GTest_LIBRARY
# GTest_MAIN_LIBRARY
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
    /opt
)

# Search the explicitly specified root first.
if(GTest_ROOT)
    find_path(GTest_INCLUDE_DIR
        gtest/gtest.h
        PATH_SUFFIXES include
        PATHS "${GTest_ROOT}"
        NO_CMAKE_FIND_ROOT_PATH
        NO_DEFAULT_PATH
    )

    find_library(GTest_LIBRARY
        NAMES gtest gtestd GTest
        PATH_SUFFIXES lib
        PATHS "${GTest_ROOT}"
        NO_CMAKE_FIND_ROOT_PATH
        NO_DEFAULT_PATH
    )

    find_library(GTest_MAIN_LIBRARY
        NAMES gtest_main gtest_maind
        PATH_SUFFIXES lib
        PATHS "${GTest_ROOT}"
        NO_CMAKE_FIND_ROOT_PATH
        NO_DEFAULT_PATH
    )
endif()

# Fall back to normal CMake search paths if necessary.
if(NOT GTest_INCLUDE_DIR)
    find_path(GTest_INCLUDE_DIR
        gtest/gtest.h
        PATH_SUFFIXES include
        PATHS ${FIND_GTest_PATHS}
    )
endif()

if(NOT GTest_LIBRARY)
    find_library(GTest_LIBRARY
        NAMES gtest gtestd GTest
        PATH_SUFFIXES lib
        PATHS ${FIND_GTest_PATHS}
    )
endif()

if(NOT GTest_MAIN_LIBRARY)
    find_library(GTest_MAIN_LIBRARY
        NAMES gtest_main gtest_maind
        PATH_SUFFIXES lib
        PATHS ${FIND_GTest_PATHS}
    )
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
    GTest
    DEFAULT_MSG
    GTest_INCLUDE_DIR
    GTest_LIBRARY
)

if(GTest_FOUND)
    set(GTest_LIBRARIES "${GTest_LIBRARY}")

    if(GTest_MAIN_LIBRARY)
        list(APPEND GTest_LIBRARIES "${GTest_MAIN_LIBRARY}")
    endif()
endif()
