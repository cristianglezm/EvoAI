set(SRC_TEST_ROOT "${PROJECT_SOURCE_DIR}/test")

# all source files
FILE(GLOB_RECURSE RUNALLTESTS_SOURCES "${SRC_TEST_ROOT}/*.cpp")
FILE(GLOB_RECURSE RUNALLTESTS_INCLUDES "${SRC_TEST_ROOT}/*.hpp")

# define the RunAllTests target
set(RunAllTests_SRC ${RUNALLTESTS_SOURCES} ${RUNALLTESTS_INCLUDES})
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++ -static")
add_executable(RunAllTests ${RunAllTests_SRC})

include_directories(${SFML_INCLUDE_DIR})
target_link_libraries(RunAllTests ${SFML_LIBRARIES})

include_directories(${JSONBOX_INCLUDE_DIR})
target_link_libraries(RunAllTests ${JSONBOX_LIBRARIES})

target_link_libraries(RunAllTests ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES})

target_link_libraries(RunAllTests EvoAI)

add_test(RunAllTests RunAllTests)