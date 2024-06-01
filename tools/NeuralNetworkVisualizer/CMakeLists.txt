
set(SRCROOT ${PROJECT_SOURCE_DIR}/tools/NeuralNetworkVisualizer)

# all source files
set(NNVisualizer_SRC ${SRCROOT}/NeuralNetworkVisualizer.cpp ${SRCROOT}/NNRenderer.cpp ${SRCROOT}/NNRenderer.hpp)

# define the NeuralNetworkVisualizer target
add_executable(NeuralNetworkVisualizer ${NNVisualizer_SRC})

if(SFML_FOUND)
	target_include_directories(NeuralNetworkVisualizer PRIVATE ${SFML_INCLUDE_DIR})
	target_link_libraries(NeuralNetworkVisualizer PRIVATE ${SFML_LIBRARIES})
endif()

if(JsonBox_FOUND)
	target_include_directories(NeuralNetworkVisualizer PRIVATE ${JsonBox_INCLUDE_DIR})
	target_link_libraries(NeuralNetworkVisualizer PRIVATE ${JsonBox_LIBRARY})
endif()

target_link_libraries(NeuralNetworkVisualizer PRIVATE EvoAI)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    message(STATUS "NeuralNetworkVisualizer - Compiler gcc")
    target_compile_options(NeuralNetworkVisualizer PRIVATE -std=c++17 -Wall -Wextra -Wshadow)
    target_link_options(NeuralNetworkVisualizer PRIVATE -mwindows)
    if(EvoAI_BUILD_STATIC)
        target_link_options(NeuralNetworkVisualizer PRIVATE -static -static-libgcc -static-libstdc++)
    endif()
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(NeuralNetworkVisualizer PRIVATE -O3 -fexpensive-optimizations -DNDEBUG)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(NeuralNetworkVisualizer PRIVATE -g)
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(STATUS "NeuralNetworkVisualizer - Compiler clang")
    target_compile_options(NeuralNetworkVisualizer PRIVATE -std=c++17 -Wall -Wextra -Wshadow)
    target_link_options(NeuralNetworkVisualizer PRIVATE -mwindows)
    if(EvoAI_BUILD_STATIC)
        target_link_options(NeuralNetworkVisualizer PRIVATE -static -static-libgcc -static-libstdc++)
    endif()
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(NeuralNetworkVisualizer PRIVATE -O3 -DNDEBUG)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(NeuralNetworkVisualizer PRIVATE -g)
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    message(STATUS "NeuralNetworkVisualizer - Compiler MSVC")
    target_compile_options(NeuralNetworkVisualizer PRIVATE /std:c++17 /W4)
    set_target_properties(NeuralNetworkVisualizer PROPERTIES WIN32_EXECUTABLE TRUE)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(NeuralNetworkVisualizer PRIVATE /O3 /DNDEBUG)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(NeuralNetworkVisualizer PRIVATE /g)
    endif()
else()
    message(WARNING "NeuralNetworkVisualizer - Compiler not supported.")
endif()

# copy file to "build"
FILE(COPY data DESTINATION ${CMAKE_BINARY_DIR}/bin/EvoAI/.)
include(GNUInstallDirs)
install(TARGETS NeuralNetworkVisualizer 
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY data DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/EvoAI/.)
