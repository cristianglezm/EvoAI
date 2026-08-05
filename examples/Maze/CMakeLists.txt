set(SRCROOT ${PROJECT_SOURCE_DIR}/examples/Maze)

set(Maze_SRC
    ${SRCROOT}/main.cpp
    ${SRCROOT}/Args.cpp
    ${SRCROOT}/Maze.cpp
    ${SRCROOT}/Render.cpp
    ${SRCROOT}/Simulation.cpp
    ${SRCROOT}/TaskQueue.cpp
)

add_executable(Maze ${Maze_SRC})

target_compile_features(Maze PRIVATE cxx_std_17)
set_target_properties(Maze PROPERTIES CXX_EXTENSIONS OFF)

target_include_directories(Maze PRIVATE ${SRCROOT})

find_package(Threads REQUIRED)
target_link_libraries(Maze PRIVATE EvoAI Threads::Threads)

target_compile_options(Maze PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wshadow>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

target_compile_options(Maze PRIVATE
    $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:Release>>:-O3 -fexpensive-optimizations>
    $<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:Release>>:-O3>
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/O2>
    $<$<AND:$<CXX_COMPILER_ID:GNU,Clang>,$<CONFIG:Debug>>:-g>
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/Zi>
)

target_compile_definitions(Maze PRIVATE
    $<$<CONFIG:Release>:NDEBUG>
)

if(EvoAI_BUILD_STATIC)
    target_link_options(Maze PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-static -static-libgcc -static-libstdc++>
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<NOT:$<PLATFORM_ID:Darwin>>>:-static -static-libgcc -static-libstdc++>
    )
endif()

message(STATUS "Maze - Compiler: ${CMAKE_CXX_COMPILER_ID}")
if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|MSVC")
    message(WARNING "Maze - Compiler not supported.")
endif()

include(GNUInstallDirs)
install(TARGETS Maze RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
