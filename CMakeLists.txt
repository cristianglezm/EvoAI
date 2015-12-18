cmake_minimum_required(VERSION 3.1)

################################################################################
### User options.
################################################################################

option(BUILD_STATIC "TRUE to statically link to the standard libraries and dependencies, FALSE to use them as DLLs" OFF)
if(BUILD_STATIC)
	SET(SFML_STATIC_LIBRARIES TRUE)
	SET(BUILD_SHARED_LIBS FALSE)
	SET(SFML_USE_STATIC_STD_LIBS TRUE)
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

add_library(EvoAI SHARED ${EvoAI_SOURCES} ${EvoAI_INCLUDES})

################################################################################
### Testing
################################################################################

enable_testing()
add_subdirectory(test)

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

################################################################################
### Enable C++14 and warnings
################################################################################

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall")
if(WIN32)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++ -static")
endif(WIN32)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -pg")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fexpensive-optimizations")
endif()

#################################################################################
### Installing Game
#################################################################################
install(TARGETS EvoAI RUNTIME DESTINATION .
			ARCHIVE DESTINATION .)
if(NOT BUILD_STATIC AND WIN32)
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		install(FILES ${SFML_DEBUG_DLLS} DESTINATION .)
	elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
		install(FILES ${SFML_RELEASE_DLLS} DESTINATION .)
	endif()
endif(NOT BUILD_STATIC AND WIN32)

install(DIRECTORY include DESTINATION .)
