cmake_minimum_required(VERSION 3.14...3.28)

################################################################################
### User options.
################################################################################

option(EvoAI_BUILD_TESTS "Build Tests" OFF)
option(EvoAI_BUILD_FUZZERS "Build libFuzzer harnesses (Clang only, native only)" OFF)
option(EvoAI_BUILD_TOOLS "Build Tools" OFF)
option(EvoAI_BUILD_EXAMPLES "Build Examples" OFF)
option(EvoAI_BUILD_STATIC "Build static" OFF)
option(EvoAI_BUILD_DOCS "Build documentation" OFF)

################################################################################
### Project files.
################################################################################

project(EvoAI)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")
include(EvoAITargetOptions)

FILE(GLOB_RECURSE EvoAI_SOURCES "src/*.cpp")
FILE(GLOB_RECURSE EvoAI_INCLUDES "include/*.hpp")

set(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(EMSCRIPTEN)
    # Emscripten can't produce a shared EvoAI - it must be linked in statically
    set(EvoAI_BUILD_STATIC TRUE)
endif()

set(EvoAI_CONFIG "Shared")
if(EvoAI_BUILD_STATIC)
    set(EvoAI_CONFIG "Static")
    set(BUILD_SHARED_LIBS FALSE)
endif()

if(NOT EvoAI_BUILD_STATIC)
	add_library(EvoAI SHARED ${EvoAI_SOURCES} ${EvoAI_INCLUDES})
	target_compile_definitions(EvoAI PRIVATE EvoAI_EXPORTS)
	set_target_properties(EvoAI PROPERTIES DEBUG_POSTFIX -d)
else()
        add_library(EvoAI STATIC ${EvoAI_SOURCES} ${EvoAI_INCLUDES})
        target_compile_definitions(EvoAI PUBLIC EvoAI_BUILD_STATIC)
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
    if(EvoAI_BUILD_STATIC)
        set(SFML_STATIC_LIBRARIES TRUE)
    endif()
    find_package(SFML 2.6 COMPONENTS graphics window system audio QUIET)
    if(NOT SFML_FOUND AND EvoAI_BUILD_STATIC)
        message(STATUS "No static SFML found locally - fetching SFML 2.6.1 source to build it statically")
        include(FetchContent)
        set(BUILD_SHARED_LIBS FALSE CACHE BOOL "" FORCE)
        set(SFML_BUILD_NETWORK FALSE CACHE BOOL "" FORCE)
        FetchContent_Declare(SFML
            GIT_REPOSITORY https://github.com/SFML/SFML.git
            GIT_TAG        2.6.1
            GIT_SHALLOW    TRUE
        )
        FetchContent_MakeAvailable(SFML)
    endif()

    # Targets that need SFML are skipped (not fatal) when this is false, so the rest of the
    # project still configures and builds.
    if(SFML_FOUND OR TARGET sfml-graphics)
        set(EvoAI_SFML_AVAILABLE TRUE)
    else()
        set(EvoAI_SFML_AVAILABLE FALSE)
        message(STATUS
            "SFML 2.6 was not found. Targets that need it will be skipped; "
            "others are unaffected. Install SFML, or configure with "
            "-DEvoAI_BUILD_STATIC=TRUE to fetch and build it from source.")
    endif()
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

evoai_set_target_options(EvoAI)

################################################################################
### Testing
################################################################################

if(EvoAI_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif(EvoAI_BUILD_TESTS)

################################################################################
### Fuzzing
################################################################################

if(EvoAI_BUILD_FUZZERS)
    if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        message(FATAL_ERROR "EvoAI_BUILD_FUZZERS requires Clang (got ${CMAKE_CXX_COMPILER_ID}) - "
            "reconfigure with -DCMAKE_CXX_COMPILER=clang++.")
    endif()
    add_subdirectory(test/fuzz)
endif(EvoAI_BUILD_FUZZERS)

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
        target_compile_definitions(EvoAI PRIVATE "STL_LIBRARY=${CMAKE_ANDROID_STL_TYPE}")
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

if((NOT EvoAI_BUILD_STATIC AND WIN32) AND (EvoAI_BUILD_TESTS OR (EvoAI_SFML_AVAILABLE AND (EvoAI_BUILD_EXAMPLES OR EvoAI_BUILD_TOOLS))))
    if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.21")
        # normalize to forward slashes before matching PRE/POST_EXCLUDE_REGEXES (CMake 4.3+)
        if(POLICY CMP0207)
            cmake_policy(SET CMP0207 NEW)
        endif()
        get_filename_component(EvoAI_MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
        set(EvoAI_RUNTIME_DEP_DIRS $<TARGET_FILE_DIR:EvoAI> ${EvoAI_MINGW_BIN_DIR})
        if(EvoAI_SFML_AVAILABLE)
            if(TARGET sfml-graphics)
                list(APPEND EvoAI_RUNTIME_DEP_DIRS $<TARGET_FILE_DIR:sfml-graphics>)
                if(TARGET sfml-audio)
                    list(APPEND EvoAI_RUNTIME_DEP_DIRS $<TARGET_FILE_DIR:sfml-audio>)
                endif()
            elseif(SFML_INCLUDE_DIR)
                get_filename_component(EvoAI_SFML_ROOT_DIR "${SFML_INCLUDE_DIR}" DIRECTORY)
                list(APPEND EvoAI_RUNTIME_DEP_DIRS "${EvoAI_SFML_ROOT_DIR}/bin")
            endif()
        endif()
        list(REMOVE_DUPLICATES EvoAI_RUNTIME_DEP_DIRS)
        install(RUNTIME_DEPENDENCY_SET EvoAIRuntimeDeps
            PRE_EXCLUDE_REGEXES "api-ms-" "ext-ms-"
            POST_EXCLUDE_REGEXES ".*[Ss]ystem32.*\\.dll"
            DIRECTORIES ${EvoAI_RUNTIME_DEP_DIRS}
            DESTINATION ${CMAKE_INSTALL_BINDIR}
        )
    else()
        message(WARNING "CMake >= 3.21 is needed to automatically bundle runtime DLLs into "
                         "the install package; skipping (found ${CMAKE_VERSION}).")
    endif()
endif((NOT EvoAI_BUILD_STATIC AND WIN32) AND (EvoAI_BUILD_TESTS OR (EvoAI_SFML_AVAILABLE AND (EvoAI_BUILD_EXAMPLES OR EvoAI_BUILD_TOOLS))))

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
if(DEFINED EvoAI_PACKAGE_SUFFIX AND NOT "${EvoAI_PACKAGE_SUFFIX}" STREQUAL "")
    SET(CPACK_PACKAGE_FILE_NAME "EvoAI-${EvoAI_VERSION}-${EvoAI_PACKAGE_SUFFIX}")
endif()
SET(CPACK_BINARY_ZIP "ON")
SET(CPACK_SOURCE_ZIP "ON")
SET(CPACK_PACKAGE_VENDOR "Cristian Gonzalez")
SET(CPACK_PACKAGE_DESCRIPTION "EvoAI is a C++17 library to evolve and / or train **small** Neural Networks.")
SET(CPACK_PACKAGE_HOMEPAGE_URL "https://cristianglezm.github.io/EvoAI")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.md")
SET(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
SET(CPACK_RESOURCE_FILE_WELCOME "${CMAKE_SOURCE_DIR}/README.md")
include(CPack)
