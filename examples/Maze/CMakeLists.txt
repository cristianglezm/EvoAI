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

target_include_directories(Maze PRIVATE ${SRCROOT})
target_link_libraries(Maze PRIVATE EvoAI)

evoai_set_target_options(Maze THREADS)

include(GNUInstallDirs)
install(TARGETS Maze RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
