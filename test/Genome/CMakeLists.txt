
set(SRCROOT ${PROJECT_SOURCE_DIR}/test/Genome)

# all source files
set(GENOME_SRC ${SRCROOT}/GENOME.cpp)

# define the GENOME target
add_executable(GENOME ${GENOME_SRC})

target_link_libraries(GENOME EvoAI)

add_test(TEST_GENOME GENOME)