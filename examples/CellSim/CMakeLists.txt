
set(SRCROOT ${PROJECT_SOURCE_DIR}/examples/CellSim)

# define the CellSim target
add_executable(CellSim)

target_sources(CellSim
	PUBLIC
		${SRCROOT}/Tournament.hpp
		${SRCROOT}/CellSim.hpp
		${SRCROOT}/Cell.hpp
		${SRCROOT}/InfoBar.hpp
		${SRCROOT}/Options.hpp
		${SRCROOT}/Averages.hpp
		${SRCROOT}/Timer.hpp
		${SRCROOT}/Trainer.hpp
	PRIVATE
		${SRCROOT}/CellSim.cpp
		${SRCROOT}/Cell.cpp
		${SRCROOT}/InfoBar.cpp
		${SRCROOT}/Options.cpp
		${SRCROOT}/Averages.cpp
		${SRCROOT}/Timer.cpp
		${SRCROOT}/Trainer.cpp
)

if(SFML_FOUND)
	target_include_directories(CellSim PRIVATE ${SFML_INCLUDE_DIR})
	target_link_libraries(CellSim PRIVATE ${SFML_LIBRARIES})
endif()

if(JsonBox_FOUND)
	target_include_directories(CellSim PRIVATE ${JsonBox_INCLUDE_DIR})
	target_link_libraries(CellSim PRIVATE ${JsonBox_LIBRARY})
endif()

target_link_libraries(CellSim PRIVATE EvoAI)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    message(STATUS "CellSim - Compiler gcc")
    target_compile_options(CellSim PRIVATE -std=c++17 -Wall -Wextra -Wshadow)
    target_link_options(CellSim PRIVATE -mwindows)
    if(EvoAI_BUILD_STATIC)
        if(NOT APPLE)
            target_link_options(CellSim PRIVATE -static -static-libgcc -static-libstdc++)
        endif()
    endif()
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(CellSim PRIVATE -O3 -fexpensive-optimizations -DNDEBUG)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(CellSim PRIVATE -g)
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(STATUS "CellSim - Compiler clang")
    target_compile_options(CellSim PRIVATE -std=c++17 -Wall -Wextra -Wshadow)
    target_link_options(CellSim PRIVATE -mwindows)
    if(EvoAI_BUILD_STATIC)
        target_link_options(CellSim PRIVATE -static -static-libgcc -static-libstdc++)
    endif()
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(CellSim PRIVATE -O3 -DNDEBUG)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(CellSim PRIVATE -g)
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    message(STATUS "CellSim - Compiler MSVC")
    target_compile_options(CellSim PRIVATE /std:c++17 /W4)
    set_target_properties(CellSim PROPERTIES WIN32_EXECUTABLE TRUE)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(CellSim PRIVATE /O3 /DNDEBUG)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(CellSim PRIVATE /g)
    endif()
else()
    message(WARNING "CellSim - Compiler not supported.")
endif()

include(GNUInstallDirs)
install(TARGETS CellSim RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
