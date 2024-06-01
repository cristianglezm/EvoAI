
set(SRCROOT ${PROJECT_SOURCE_DIR}/examples/IrisClassification)

# all source files
set(IrisClassification_SRC ${SRCROOT}/IrisClassification.cpp)

# define the IrisClassification target
add_executable(IrisClassification ${IrisClassification_SRC})

target_link_libraries(IrisClassification PRIVATE EvoAI)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    message(STATUS "IrisClassification - Compiler gcc")
    target_compile_options(IrisClassification PRIVATE -std=c++17 -Wall -Wextra -Wshadow)
    if(EvoAI_BUILD_STATIC)
        target_link_options(IrisClassification PRIVATE -static -static-libgcc -static-libstdc++)
    endif()
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(IrisClassification PRIVATE -O3 -fexpensive-optimizations -DNDEBUG)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(IrisClassification PRIVATE -g)
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(STATUS "IrisClassification - Compiler clang")
    target_compile_options(IrisClassification PRIVATE -std=c++17 -Wall -Wextra -Wshadow)
    if(EvoAI_BUILD_STATIC)
        target_link_options(IrisClassification PRIVATE -static -static-libgcc -static-libstdc++)
    endif()
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(IrisClassification PRIVATE -O3 -DNDEBUG)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(IrisClassification PRIVATE -g)
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    message(STATUS "IrisClassification - Compiler MSVC")
    target_compile_options(IrisClassification PRIVATE /std:c++17 /W4)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(IrisClassification PRIVATE /O3 /DNDEBUG)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(IrisClassification PRIVATE /g)
    endif()
else()
    message(WARNING "IrisClassification - Compiler not supported.")
endif()

# copy to "build"
FILE(COPY_FILE iris.data.csv ${CMAKE_BINARY_DIR}/iris.data.csv ONLY_IF_DIFFERENT)
include(GNUInstallDirs)
install(TARGETS IrisClassification RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES iris.data.csv DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/EvoAI/data/.)