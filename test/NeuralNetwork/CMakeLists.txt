
set(SRCROOT ${PROJECT_SOURCE_DIR}/test/NeuralNetwork)

# all source files
set(NeuralNetwork_SRC ${SRCROOT}/NeuralNetwork.cpp)

# define the NeuralNetwork target
add_executable(NeuralNetwork ${NeuralNetwork_SRC})

target_link_libraries(NeuralNetwork EvoAI)

add_test(TEST_NeuralNetwork NeuralNetwork)