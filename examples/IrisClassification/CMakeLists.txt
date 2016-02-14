
set(SRCROOT ${PROJECT_SOURCE_DIR}/examples/IrisClassification)

# all source files
set(IrisClassification_SRC ${SRCROOT}/IrisClassification.cpp)

# define the IrisClassification target
add_executable(IrisClassification ${IrisClassification_SRC})

target_link_libraries(IrisClassification EvoAI)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -pg")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fexpensive-optimizations")
endif()

if(NOT BUILD_STATIC)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++ -static")
endif(NOT BUILD_STATIC)

install(TARGETS IrisClassification RUNTIME DESTINATION bin ARCHIVE DESTINATION bin)
install(FILES iris.data.csv DESTINATION bin)