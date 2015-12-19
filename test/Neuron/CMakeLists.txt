
set(SRCROOT ${PROJECT_SOURCE_DIR}/test/Neuron)

# all source files
set(Neuron_SRC ${SRCROOT}/Neuron.cpp)

# define the Neuron target
add_executable(Neuron ${Neuron_SRC})

target_link_libraries(Neuron EvoAI)

add_test(TEST_Neuron Neuron)