cmake_minimum_required(VERSION 3.14...3.27)

################################################################################
### User options.
################################################################################

option(EvoAI_BUILD_TESTS "Build Tests" OFF)
option(EvoAI_BUILD_TOOLS "Build Tools" OFF)
option(EvoAI_BUILD_EXAMPLES "Build Examples" OFF)
option(EvoAI_BUILD_STATIC "Build static" OFF)
option(EvoAI_BUILD_DOCS "Build documentation" OFF)

set(EvoAI_CONFIG "Shared")
if(EvoAI_BUILD_STATIC)
    set(EvoAI_CONFIG "Static")
    SET(BUILD_SHARED_LIBS FALSE)
    add_definitions("-DEvoAI_BUILD_STATIC")
endif(EvoAI_BUILD_STATIC)

################################################################################
### Project files.
################################################################################

project(EvoAI)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

FILE(GLOB_RECURSE EvoAI_SOURCES "src/*.cpp")
FILE(GLOB_RECURSE EvoAI_INCLUDES "include/*.hpp")

set(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(EMSCRIPTEN)
    set(EvoAI_BUILD_STATIC TRUE)
endif(EMSCRIPTEN)

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
target_include_directories(EvoAI PUBLIC
                            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                            "$<INSTALL_INTERFACE:include>"
                            )
target_sources(EvoAI
    PRIVATE
        ${EvoAI_SOURCES}
    PUBLIC
        "$<BUILD_INTERFACE:${EvoAI_INCLUDES}>"
        "$<INSTALL_INTERFACE:include>"
)

SET(EvoAI_CONFIG_HPP_INPUT "${CMAKE_SOURCE_DIR}/include/EvoAI/Config.hpp")
FILE(READ "${EvoAI_CONFIG_HPP_INPUT}" EvoAI_CONFIG_HPP_CONTENTS)
STRING(REGEX REPLACE "^.*#define EVOAI_VERSION_MAJOR ([0-9]+).*$" "\\1" EvoAI_VERSION_MAJOR "${EvoAI_CONFIG_HPP_CONTENTS}")
STRING(REGEX REPLACE "^.*#define EVOAI_VERSION_MINOR ([0-9]+).*$" "\\1" EvoAI_VERSION_MINOR "${EvoAI_CONFIG_HPP_CONTENTS}")
STRING(REGEX REPLACE "^.*#define EVOAI_VERSION_PATCH ([0-9]+).*$" "\\1" EvoAI_VERSION_PATCH "${EvoAI_CONFIG_HPP_CONTENTS}")
SET(EvoAI_VERSION ${EvoAI_VERSION_MAJOR}.${EvoAI_VERSION_MINOR}.${EvoAI_VERSION_PATCH})
set_target_properties(EvoAI PROPERTIES VERSION ${EvoAI_VERSION} SOVERSION ${EvoAI_VERSION_MAJOR})
set_target_properties(EvoAI PROPERTIES OUTPUT_NAME "EvoAI-${EvoAI_VERSION_MAJOR}")
set_property(TARGET EvoAI PROPERTY INTERFACE_EvoAI_MAJOR_VERSION ${EvoAI_VERSION_MAJOR})
set_property(TARGET EvoAI APPEND PROPERTY
  COMPATIBLE_INTERFACE_STRING ${EvoAI_VERSION_MAJOR}
)

################################################################################
### Dependencies.
################################################################################

include(CMakeFindDependencyMacro)
# JsonBox
find_dependency(JsonBox REQUIRED)
if(JsonBox_FOUND)
    target_include_directories(EvoAI PUBLIC 
        "$<BUILD_INTERFACE:${JsonBox_INCLUDE_DIR}>" 
        "$<INSTALL_INTERFACE:include>")
    target_link_libraries(EvoAI PUBLIC ${JsonBox_LIBRARY})
endif(JsonBox_FOUND)

if(EvoAI_BUILD_EXAMPLES OR EvoAI_BUILD_TOOLS)
    #sfml
    find_package(SFML 2.6 COMPONENTS graphics window system audio REQUIRED)
endif(EvoAI_BUILD_EXAMPLES OR EvoAI_BUILD_TOOLS)

if(EvoAI_BUILD_TESTS)
    find_package(GTest REQUIRED)
    #find_package(GMock REQUIRED)
endif(EvoAI_BUILD_TESTS)

################################################################################
### Enable C++17 and warnings
################################################################################

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    message(STATUS "EvoAI - Compiler gcc")
    target_compile_options(EvoAI PRIVATE -std=c++17 -Wall -Wextra -Wshadow)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(EvoAI PRIVATE -O3 -fexpensive-optimizations -DNDEBUG)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(EvoAI PRIVATE -g)
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(STATUS "EvoAI - Compiler clang")
    target_compile_options(EvoAI PRIVATE -std=c++17 -Wall -Wextra -Wshadow)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(EvoAI PRIVATE -O3 -DNDEBUG)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(EvoAI PRIVATE -g)
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    message(STATUS "EvoAI - Compiler MSVC")
    target_compile_options(EvoAI PRIVATE /std:c++17 /W4)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(EvoAI PRIVATE /O3 /DNDEBUG)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(EvoAI PRIVATE /g)
    endif()
else()
    message(WARNING "EvoAI - Compiler not supported.")
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
### Emscripten
#################################################################################

if(EMSCRIPTEN)
    set(EvoAI_BUILD_STATIC TRUE)
    message(STATUS "Compiling for Emscripten")
    target_compile_options(EvoAI PRIVATE -Wall -Wextra -Wshadow -pthread -fexceptions)
    target_link_options(EvoAI PRIVATE -sPTHREAD_POOL_SIZE=4 -fexceptions)
endif(EMSCRIPTEN)

#################################################################################
### Android
#################################################################################

# set options for android
if(CMAKE_SYSTEM_NAME MATCHES "Android")
    message(STATUS "Compiling for Android")
    if(CMAKE_ANDROID_API LESS 14)
        message(FATAL_ERROR "Android API level (${CMAKE_ANDROID_API}) must be equal or greater than 14.")
    endif()
    if(CMAKE_VS_PLATFORM_NAME STREQUAL "Tegra-Android")
        message(WARNING "CMake might not properly support setting the STL. Make sure to adjust all generated library projects!")
    endif()
	set(ANDROID_NDK "$ENV{ANDROID_NDK}")
    set(CMAKE_INSTALL_PREFIX ${CMAKE_ANDROID_NDK}/sources/third_party/EvoAI)
    set(DEFAULT_INSTALL_MISC_DIR ${CMAKE_ANDROID_NDK}/sources/third_party/EvoAI)
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
    add_subdirectory(docs)
endif(EvoAI_BUILD_DOCS)

#################################################################################
### Installing Library
#################################################################################

include(GNUInstallDirs)
# LIB_SUFFIX is for android arch
install(TARGETS EvoAI 
        EXPORT EvoAI${EvoAI_CONFIG}Targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}${LIB_SUFFIX}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}${LIB_SUFFIX}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY include DESTINATION .)
install(FILES LICENSE.md DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/EvoAI)
install(FILES tools/SoundGenerator/midifile/Midifile_LICENSE.txt DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/EvoAI)
install(FILES README.md DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/EvoAI)
if(EvoAI_BUILD_DOCS)
    install(DIRECTORY "${CMAKE_BINARY_DIR}/docs/sphinx/html" DESTINATION "${CMAKE_INSTALL_DOCDIR}/v${EvoAI_VERSION_MAJOR}.${EvoAI_VERSION_MINOR}")
endif(EvoAI_BUILD_DOCS)

if(WIN32)
    FILE(GLOB_RECURSE SFML_DEBUG_DLLS "${SFML_ROOT}/bin/sfml-*-d-2.dll")
    FILE(GLOB_RECURSE SFML_RELEASE_DLLS "${SFML_ROOT}/bin/*-2.dll")
    if(NOT SFML_DEBUG_DLLS STREQUAL "")
        LIST(REMOVE_ITEM SFML_RELEASE_DLLS ${SFML_DEBUG_DLLS})
    endif(NOT SFML_DEBUG_DLLS STREQUAL "")
endif(WIN32)

if((NOT EvoAI_BUILD_STATIC AND WIN32) AND (EvoAI_BUILD_EXAMPLES OR EvoAI_BUILD_TOOLS))
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		install(FILES ${SFML_DEBUG_DLLS} DESTINATION ${CMAKE_INSTALL_BINDIR})
	elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
		install(FILES ${SFML_RELEASE_DLLS} DESTINATION ${CMAKE_INSTALL_BINDIR})
	endif()
endif((NOT EvoAI_BUILD_STATIC AND WIN32) AND (EvoAI_BUILD_EXAMPLES OR EvoAI_BUILD_TOOLS))

###################################################################################
### Config CMake
###################################################################################

include(CMakePackageConfigHelpers)
export(EXPORT EvoAI${EvoAI_CONFIG}Targets
       FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/EvoAI${EvoAI_CONFIG}Targets.cmake"
       NAMESPACE EvoAI::
)
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/EvoAIConfig.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/EvoAI
)
install(EXPORT EvoAI${EvoAI_CONFIG}Targets
        FILE EvoAI${EvoAI_CONFIG}Targets.cmake
        NAMESPACE EvoAI::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/EvoAI
)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/EvoAIConfigVersion.cmake"
  VERSION "${EvoAI_VERSION}"
  COMPATIBILITY AnyNewerVersion
)
install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/EvoAIConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/EvoAIConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/EvoAI
)

###################################################################################
### CPACK Config
###################################################################################

SET(CPACK_GENERATOR "ZIP")
SET(CPACK_PACKAGE_VERSION_MAJOR ${EvoAI_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${EvoAI_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${EvoAI_VERSION_PATCH})
SET(CPACK_BINARY_ZIP "ON")
SET(CPACK_SOURCE_ZIP "ON")
SET(CPACK_PACKAGE_VENDOR "Cristian Gonzalez")
SET(CPACK_PACKAGE_DESCRIPTION "EvoAI is a C++17 library to evolve and / or train **small** Neural Networks.")
SET(CPACK_PACKAGE_HOMEPAGE_URL "https://cristianglezm.github.io/EvoAI")
SET(CPACK_RESOURCE_FILE_LICENSE "../LICENSE.md")
SET(CPACK_RESOURCE_FILE_README "../README.md")
SET(CPACK_RESOURCE_FILE_WELCOME "../README.md")
include(CPack)
