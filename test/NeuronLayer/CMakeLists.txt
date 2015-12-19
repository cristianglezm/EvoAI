
set(SRCROOT ${PROJECT_SOURCE_DIR}/test/NeuronLayer)

# all source files
set(NeuronLayer_SRC ${SRCROOT}/NeuronLayer.cpp)

# define the XOR target
add_executable(NeuronLayer ${NeuronLayer_SRC})

target_link_libraries(NeuronLayer EvoAI)

add_test(TEST_NeuronLayer NeuronLayer)