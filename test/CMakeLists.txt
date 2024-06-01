set(SRC_TEST_ROOT "${PROJECT_SOURCE_DIR}/test")

# all source files
FILE(GLOB_RECURSE RUNALLTESTS_SOURCES "${SRC_TEST_ROOT}/*.cpp")
FILE(GLOB_RECURSE RUNALLTESTS_INCLUDES "${SRC_TEST_ROOT}/*.hpp")

# define the RunAllTests target
set(RunAllTests_SRC ${RUNALLTESTS_SOURCES} ${RUNALLTESTS_INCLUDES})
add_executable(RunAllTests ${RunAllTests_SRC})

target_sources(RunAllTests
    PUBLIC
        ${RUNALLTESTS_INCLUDES}
    PRIVATE
        ${RUNALLTESTS_SOURCES}
)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)

target_include_directories(RunAllTests PRIVATE ${GTest_INCLUDE_DIR} ${JsonBox_INCLUDE_DIR})
target_link_libraries(RunAllTests PRIVATE ${GTest_LIBRARIES} EvoAI ${JsonBox_LIBRARY})

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    message(STATUS "RunAllTests - Compiler gcc")
    target_compile_options(RunAllTests PRIVATE -std=c++17 -Wall -Wextra -Wshadow)
    if(EvoAI_BUILD_STATIC)
        target_link_options(RunAllTests PRIVATE -static -static-libgcc -static-libstdc++)
    endif()
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(RunAllTests PRIVATE -O3 -fexpensive-optimizations -DNDEBUG)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(RunAllTests PRIVATE -g)
    endif()
elseif(EMSCRIPTEN)
    message(STATUS "RunAllTests - Compiler EMSCRIPTEM")
    target_compile_options(RunAllTests PRIVATE -std=c++17 -Wall -Wextra -Wshadow -pthread -fexceptions)
    target_link_options(RunAllTests PRIVATE -sPTHREAD_POOL_SIZE=4 -fexceptions)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(RunAllTests PRIVATE -O3 -DNDEBUG)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(RunAllTests PRIVATE -g)
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(STATUS "RunAllTests - Compiler clang")
    target_compile_options(RunAllTests PRIVATE -std=c++17 -Wall -Wextra -Wshadow)
    if(EvoAI_BUILD_STATIC)
        target_link_options(RunAllTests PRIVATE -static -static-libgcc -static-libstdc++)
    endif()
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(RunAllTests PRIVATE -O3 -DNDEBUG)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(RunAllTests PRIVATE -g)
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    message(STATUS "RunAllTests - Compiler MSVC")
    target_compile_options(RunAllTests PRIVATE /std:c++17 /W4)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(RunAllTests PRIVATE /O3 /DNDEBUG)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(RunAllTests PRIVATE /g)
    endif()
else()
    message(WARNING "RunAllTests - Compiler not supported.")
endif()

include(GNUInstallDirs)
install(TARGETS RunAllTests 
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

include(GoogleTest)
if(NOT CMAKE_SYSTEM_NAME MATCHES "Android")
    gtest_discover_tests(RunAllTests "../bin/RunAllTests")
endif(NOT CMAKE_SYSTEM_NAME MATCHES "Android")
