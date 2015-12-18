
set(SRCROOT ${PROJECT_SOURCE_DIR}/test/NeuralNetworkVisualizer)

# all source files
set(SRC ${SRCROOT}/NeuralNetworkVisualizer.cpp)

# define the NeuralNetworkVisualizer target
add_executable(NeuralNetworkVisualizer SOURCES ${SRC} DEPENDS SFML JSONBOX)
