
set(SRCROOT ${PROJECT_SOURCE_DIR}/test/XOR)

# all source files
set(XOR_SRC ${SRCROOT}/XOR.cpp)

# define the XOR target
add_executable(XOR ${XOR_SRC})

target_link_libraries(XOR EvoAI)

add_test(TEST_XOR XOR)