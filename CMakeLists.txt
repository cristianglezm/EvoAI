cmake_minimum_required(VERSION 3.7.2...3.14)

################################################################################
### User options.
################################################################################

option(EvoAI_BUILD_TESTS "Build Tests" OFF)
option(EvoAI_BUILD_TOOLS "Build Tools" OFF)
option(EvoAI_BUILD_EXAMPLES "Build Examples" OFF)
option(EvoAI_BUILD_STATIC "Build static" OFF)
option(EvoAI_BUILD_DOCS "Build documentation" OFF)

if(EvoAI_BUILD_STATIC)
    SET(BUILD_SHARED_LIBS FALSE)
    add_definitions("-DEvoAI_BUILD_STATIC")
endif(EvoAI_BUILD_STATIC)

################################################################################
### Project files.
################################################################################

project(EvoAI)

SET(CMAKE_INCLUDE_PATH ${CMAKE_SOURCE_DIR})
include_directories("${CMAKE_SOURCE_DIR}/include")
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

FILE(GLOB_RECURSE EvoAI_SOURCES "src/*.cpp")
FILE(GLOB_RECURSE EvoAI_INCLUDES "include/*.hpp")

set(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")

if(NOT EvoAI_BUILD_STATIC)
	add_definitions("-DEvoAI_EXPORTS")
	add_library(EvoAI SHARED ${EvoAI_SOURCES} ${EvoAI_INCLUDES})
	set_target_properties(EvoAI PROPERTIES DEBUG_POSTFIX -d)
else()
	add_library(EvoAI STATIC ${EvoAI_SOURCES} ${EvoAI_INCLUDES})
        set_target_properties(EvoAI PROPERTIES DEBUG_POSTFIX -s-d)
	set_target_properties(EvoAI PROPERTIES RELEASE_POSTFIX -s)
	set_target_properties(EvoAI PROPERTIES MINSIZEREL_POSTFIX -s)
endif(NOT EvoAI_BUILD_STATIC)

SET(EvoAI_CONFIG_HPP_INPUT "${CMAKE_SOURCE_DIR}/include/EvoAI/Config.hpp")
FILE(READ "${EvoAI_CONFIG_HPP_INPUT}" EvoAI_CONFIG_HPP_CONTENTS)
STRING(REGEX REPLACE "^.*#define EVOAI_VERSION_MAJOR ([0-9]+).*$" "\\1" EvoAI_VERSION_MAJOR "${EvoAI_CONFIG_HPP_CONTENTS}")
STRING(REGEX REPLACE "^.*#define EVOAI_VERSION_MINOR ([0-9]+).*$" "\\1" EvoAI_VERSION_MINOR "${EvoAI_CONFIG_HPP_CONTENTS}")
STRING(REGEX REPLACE "^.*#define EVOAI_VERSION_PATCH ([0-9]+).*$" "\\1" EvoAI_VERSION_PATCH "${EvoAI_CONFIG_HPP_CONTENTS}")
SET(EvoAI_VERSION ${EvoAI_VERSION_MAJOR}.${EvoAI_VERSION_MINOR}.${EvoAI_VERSION_PATCH})
set_target_properties(EvoAI PROPERTIES VERSION ${EvoAI_VERSION} SOVERSION ${EvoAI_VERSION_MAJOR})

set_target_properties(EvoAI PROPERTIES OUTPUT_NAME "EvoAI-${EvoAI_VERSION_MAJOR}")

################################################################################
### Dependencies.
################################################################################

#jsonbox
find_package(JsonBox REQUIRED)
if(JsonBox_FOUND)
	include_directories(${JsonBox_INCLUDE_DIR})
	target_link_libraries(EvoAI ${JsonBox_LIBRARIES})
endif(JsonBox_FOUND)

if(EvoAI_BUILD_TESTS)
	find_package(GTest REQUIRED)
	find_package(GMock REQUIRED)
endif(EvoAI_BUILD_TESTS)

################################################################################
### Enable C++17 and warnings
################################################################################

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

if(NOT CMAKE_SYSTEM_NAME MATCHES "Android")
    SET(CMAKE_CXX_STANDARD 17)
    SET(CMAKE_CXX_STANDARD_REQUIRED ON)
    SET(CMAKE_CXX_EXTENSIONS OFF)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Wextra")
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fexpensive-optimizations")
    endif()
endif(NOT CMAKE_SYSTEM_NAME MATCHES "Android")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_DEBUG_POSTFIX "_d")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

################################################################################
### Testing
################################################################################

if(EvoAI_BUILD_TESTS)
	enable_testing()
	add_subdirectory(test)
endif(EvoAI_BUILD_TESTS)

################################################################################
### examples
################################################################################

if(EvoAI_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif(EvoAI_BUILD_EXAMPLES)

################################################################################
### Tools
################################################################################

if(EvoAI_BUILD_TOOLS)
	add_subdirectory(tools)
endif(EvoAI_BUILD_TOOLS)

#################################################################################
### Android
#################################################################################

# set options for android
if(CMAKE_SYSTEM_NAME MATCHES "Android")
	message("-- Compiling for Android")
    set(BUILD_SHARED_LIBS TRUE)
    if (CMAKE_ANDROID_API LESS 14)
        message(FATAL_ERROR "Android API level (${CMAKE_ANDROID_API}) must be equal or greater than 14.")
    endif()
    if(CMAKE_VS_PLATFORM_NAME STREQUAL "Tegra-Android")
        message(WARNING "CMake might not properly support setting the STL. Make sure to adjust all generated library projects!")
    endif()
	set(ANDROID_NDK "$ENV{ANDROID_NDK}")
    set(CMAKE_INSTALL_PREFIX ${CMAKE_ANDROID_NDK}/sources/third_party/evoai)
    set(DEFAULT_INSTALL_MISC_DIR ${CMAKE_ANDROID_NDK}/sources/third_party/evoai)
    set(LIB_SUFFIX "/${CMAKE_ANDROID_ARCH_ABI}")
    if(CMAKE_ANDROID_STL_TYPE MATCHES "_shared")
        add_definitions("-DSTL_LIBRARY=${CMAKE_ANDROID_STL_TYPE}")
        if(NOT CMAKE_ANDROID_STL_TYPE MATCHES "c\\+\\+_shared")
            message("Android: Using ${CMAKE_ANDROID_STL_TYPE} as STL. Set CMAKE_ANDROID_STL_TYPE to c++_shared, if there are any issues.")
        endif()
    else()
        message(WARNING "Android: You're using a static STL (${CMAKE_ANDROID_STL_TYPE}). Set CMAKE_ANDROID_STL_TYPE to c++_shared, if there are any issues.")
    endif()
	install(FILES src/Android.mk DESTINATION .)
endif(CMAKE_SYSTEM_NAME MATCHES "Android")

#################################################################################
### Docs
#################################################################################

if(EvoAI_BUILD_DOCS)
    find_package(Doxygen)
    IF(DOXYGEN_FOUND)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doxyfile.in 
			${CMAKE_CURRENT_BINARY_DIR}/doxyfile @ONLY)
        SET(DOXYGEN_INPUT "${CMAKE_CURRENT_BINARY_DIR}/doxyfile")
        SET(DOXYGEN_OUTPUT "${CMAKE_SOURCE_DIR}/docs")

        ADD_CUSTOM_COMMAND(
          OUTPUT ${DOXYGEN_OUTPUT}
          COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
          COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
          COMMAND ${CMAKE_COMMAND} -E echo "Done."
          DEPENDS ${DOXYGEN_INPUT})

        ADD_CUSTOM_TARGET(docs ALL DEPENDS ${DOXYGEN_OUTPUT})

        ADD_CUSTOM_TARGET(docs_forced
          COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
          COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
          COMMAND ${CMAKE_COMMAND} -E echo "Done.")
    ENDIF(DOXYGEN_FOUND)
endif(EvoAI_BUILD_DOCS)

#################################################################################
### Installing Library
#################################################################################

install(TARGETS EvoAI LIBRARY DESTINATION lib RUNTIME DESTINATION bin
                        ARCHIVE DESTINATION lib)
install(FILES LICENSE.md DESTINATION share/EvoAI)
install(FILES tools/SoundGenerator/midifile/Midifile_LICENSE.txt DESTINATION share/EvoAI)
install(FILES README.md DESTINATION share/EvoAI)
if(EvoAI_BUILD_DOCS)
    install(DIRECTORY docs DESTINATION share/EvoAI)
endif(EvoAI_BUILD_DOCS)
install(DIRECTORY include DESTINATION .)
