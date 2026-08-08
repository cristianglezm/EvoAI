set(SRC_TEST_ROOT "${PROJECT_SOURCE_DIR}/test")

# all source files
FILE(GLOB_RECURSE RUNALLTESTS_SOURCES "${SRC_TEST_ROOT}/*.cpp")
FILE(GLOB_RECURSE RUNALLTESTS_INCLUDES "${SRC_TEST_ROOT}/*.hpp")

# define the RunAllTests target
set(RunAllTests_SRC ${RUNALLTESTS_SOURCES} ${RUNALLTESTS_INCLUDES})
add_executable(RunAllTests ${RunAllTests_SRC})

target_sources(RunAllTests
    PUBLIC
        ${RUNALLTESTS_INCLUDES}
    PRIVATE
        ${RUNALLTESTS_SOURCES}
)

target_include_directories(RunAllTests PRIVATE ${GTest_INCLUDE_DIR} ${JsonBox_INCLUDE_DIR})
target_link_libraries(RunAllTests PRIVATE ${GTest_LIBRARIES} EvoAI ${JsonBox_LIBRARY})

evoai_set_target_options(RunAllTests)

if(EMSCRIPTEN)
    target_compile_options(RunAllTests PRIVATE -pthread -fexceptions)
    target_link_options(RunAllTests PRIVATE -sPTHREAD_POOL_SIZE=4 -fexceptions)
endif()

include(GNUInstallDirs)
install(TARGETS RunAllTests 
        RUNTIME_DEPENDENCY_SET EvoAIRuntimeDeps
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

include(GoogleTest)
if(NOT CMAKE_SYSTEM_NAME MATCHES "Android")
    gtest_discover_tests(RunAllTests "../bin/RunAllTests")
endif(NOT CMAKE_SYSTEM_NAME MATCHES "Android")
