
set(SRCROOT ${PROJECT_SOURCE_DIR}/examples/XOR)

# all source files
set(XOR_SRC ${SRCROOT}/XOR.cpp)

# define the XOR target
add_executable(XOR ${XOR_SRC})

target_link_libraries(XOR EvoAI)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -pg")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fexpensive-optimizations")
endif()

if(NOT BUILD_STATIC)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++ -static")
endif(NOT BUILD_STATIC)

install(TARGETS XOR RUNTIME DESTINATION bin ARCHIVE DESTINATION bin)