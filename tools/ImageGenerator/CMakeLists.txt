
set(SRCROOT ${PROJECT_SOURCE_DIR}/tools/ImageGenerator)

# all source files
set(ImageGenerator_SRC ${SRCROOT}/ImageGenerator.cpp)

# define the NeuralNetworkVisualizer target
add_executable(ImageGenerator ${ImageGenerator_SRC})

include_directories(${SFML_INCLUDE_DIR})
target_link_libraries(ImageGenerator ${SFML_LIBRARIES})

include_directories(${JSONBOX_INCLUDE_DIR})
target_link_libraries(ImageGenerator ${JSONBOX_LIBRARIES})
target_link_libraries(ImageGenerator EvoAI)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -pg")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fexpensive-optimizations")
endif()

if(NOT BUILD_STATIC)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++ -static")
endif(NOT BUILD_STATIC)

install(TARGETS ImageGenerator RUNTIME DESTINATION . ARCHIVE DESTINATION .)