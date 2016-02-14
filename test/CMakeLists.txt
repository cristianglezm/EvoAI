set(SRC_TEST_ROOT "${PROJECT_SOURCE_DIR}/test")

# all source files
FILE(GLOB_RECURSE RUNALLTESTS_SOURCES "${SRC_TEST_ROOT}/*.cpp")
FILE(GLOB_RECURSE RUNALLTESTS_INCLUDES "${SRC_TEST_ROOT}/*.hpp")

# define the RunAllTests target
set(RunAllTests_SRC ${RUNALLTESTS_SOURCES} ${RUNALLTESTS_INCLUDES})

add_executable(RunAllTests ${RunAllTests_SRC})

include_directories(${SFML_INCLUDE_DIR})
target_link_libraries(RunAllTests ${SFML_LIBRARIES})

include_directories(${JSONBOX_INCLUDE_DIR})

target_link_libraries(RunAllTests ${JSONBOX_LIBRARIES})
include_directories(${GTEST_INCLUDE_DIR})
include_directories(${GMOCK_INCLUDE_DIR})
target_link_libraries(RunAllTests ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES})
target_link_libraries(RunAllTests EvoAI)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -pg")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fexpensive-optimizations")
endif()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++ -static")

add_test(RunAllTests RunAllTests)