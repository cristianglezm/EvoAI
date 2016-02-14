cmake_minimum_required(VERSION 3.1)

################################################################################
### User options.
################################################################################

option(BUILD_TESTS "Build Tests" OFF)
option(BUILD_TOOLS "Build Tools" OFF)
option(BUILD_EXAMPLES "Build Examples" OFF)
option(BUILD_STATIC "Build static" OFF)
option(BUILD_DOCS "Build documentation" OFF)

if(BUILD_STATIC)
	SET(SFML_STATIC_LIBRARIES TRUE)
	SET(BUILD_SHARED_LIBS FALSE)
	SET(SFML_USE_STATIC_STD_LIBS TRUE)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++ -static")
endif(BUILD_STATIC)

################################################################################
### Project files.
################################################################################

project(EvoAI)

SET(CMAKE_INCLUDE_PATH ${CMAKE_SOURCE_DIR})
include_directories("${CMAKE_SOURCE_DIR}/include")
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

FILE(GLOB_RECURSE EvoAI_SOURCES "src/*.cpp")
FILE(GLOB_RECURSE EvoAI_INCLUDES "include/*.hpp")

set(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib")
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")

if(NOT BUILD_STATIC)
	add_library(EvoAI SHARED ${EvoAI_SOURCES} ${EvoAI_INCLUDES})
else()
	add_library(EvoAI STATIC ${EvoAI_SOURCES} ${EvoAI_INCLUDES})
endif(NOT BUILD_STATIC)

################################################################################
### Dependencies.
################################################################################

#sfml
find_package(SFML 2.1 COMPONENTS graphics window system network audio REQUIRED)
if(SFML_FOUND)
	include_directories(${SFML_INCLUDE_DIR})
	target_link_libraries(EvoAI ${SFML_LIBRARIES})
endif(SFML_FOUND)

if(WIN32)
	FILE(GLOB_RECURSE SFML_DEBUG_DLLS "${SFML_ROOT}/bin/sfml-*-d-2.dll")
	FILE(GLOB_RECURSE SFML_RELEASE_DLLS "${SFML_ROOT}/bin/*-2.dll")
	LIST(REMOVE_ITEM SFML_RELEASE_DLLS ${SFML_DEBUG_DLLS})
endif(WIN32)

#jsonbox
find_package(JSONBOX REQUIRED)
if(JSONBOX_FOUND)
	include_directories(${JSONBOX_INCLUDE_DIR})
	target_link_libraries(EvoAI ${JSONBOX_LIBRARIES})
endif(JSONBOX_FOUND)

if(BUILD_TESTS)
	find_package(GTest REQUIRED)
	find_package(GMock REQUIRED)
endif(BUILD_TESTS)

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

if(BUILD_TESTS)
	enable_testing()
	add_subdirectory(test)
endif(BUILD_TESTS)

################################################################################
### examples
################################################################################

if(BUILD_EXAMPLES)
	add_subdirectory(examples)
endif(BUILD_EXAMPLES)

################################################################################
### Tools
################################################################################

if(BUILD_TOOLS)
	add_subdirectory(tools)
endif(BUILD_TOOLS)

#################################################################################
### Docs
#################################################################################

if(BUILD_DOCS)
    find_package(Doxygen)
    IF(DOXYGEN_FOUND)
        SET(DOXYGEN_INPUT "${CMAKE_SOURCE_DIR}/doxyfile")
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
endif(BUILD_DOCS)

#################################################################################
### Installing Library
#################################################################################

install(TARGETS EvoAI LIBRARY DESTINATION bin RUNTIME DESTINATION bin
						ARCHIVE DESTINATION lib)
install(FILES LICENSE.txt DESTINATION .)
install(FILES README.md DESTINATION .)
if(BUILD_DOCS)
    install(DIRECTORY docs DESTINATION .)
endif(BUILD_DOCS)
install(DIRECTORY include DESTINATION .)

if(NOT BUILD_STATIC AND WIN32)
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		install(FILES ${SFML_DEBUG_DLLS} DESTINATION bin)
	elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
		install(FILES ${SFML_RELEASE_DLLS} DESTINATION bin)
	endif()
endif(NOT BUILD_STATIC AND WIN32)