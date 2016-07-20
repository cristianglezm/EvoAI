cmake_minimum_required(VERSION 3.1)

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
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

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
find_package(JSONBOX REQUIRED)
if(JSONBOX_FOUND)
	include_directories(${JSONBOX_INCLUDE_DIR})
	target_link_libraries(EvoAI ${JSONBOX_LIBRARIES})
endif(JSONBOX_FOUND)

if(EvoAI_BUILD_TESTS)
	find_package(GTest REQUIRED)
	find_package(GMock REQUIRED)
endif(EvoAI_BUILD_TESTS)

################################################################################
### Enable C++14 and warnings
################################################################################

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall")
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_DEBUG_POSTFIX "_d")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fexpensive-optimizations")
endif()

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

if(ANDROID_ABI)
	message("-- Compiling for Android")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -DANDROID -DUSE_MAKE_UNIQUE")
	set(ANDROID_API_MIN 9)
	set(ANDROID_NATIVE_API_LEVEL ${ANDROID_API_MIN})
	set(ANDROID_NDK "$ENV{ANDROID_NDK}")
	set(CMAKE_INSTALL_PREFIX "${ANDROID_NDK}/sources/EvoAI")
	set(ANDROID_STL "c++_shared")
	set(LIB_SUFFIX "/${ANDROID_ABI}")
	if(ANDROID_STL MATCHES "_shared")
	        add_definitions("-DSTL_LIBRARY=${ANDROID_STL}")
	endif()
	install(FILES src/Android.mk DESTINATION .)
endif(ANDROID_ABI)

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

install(TARGETS EvoAI LIBRARY DESTINATION bin RUNTIME DESTINATION bin
                        ARCHIVE DESTINATION lib)
install(FILES LICENSE.txt DESTINATION .)
install(FILES README.md DESTINATION .)
if(EvoAI_BUILD_DOCS)
    install(DIRECTORY docs DESTINATION .)
endif(EvoAI_BUILD_DOCS)
install(DIRECTORY include DESTINATION .)
