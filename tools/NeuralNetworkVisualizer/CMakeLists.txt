
set(SRCROOT ${PROJECT_SOURCE_DIR}/tools/NeuralNetworkVisualizer)

# all source files
set(NNVisualizer_SRC ${SRCROOT}/NeuralNetworkVisualizer.cpp ${SRCROOT}/NNRenderer.cpp ${SRCROOT}/NNRenderer.hpp)

# define the NeuralNetworkVisualizer target
add_executable(NeuralNetworkVisualizer ${NNVisualizer_SRC})

if(SFML_FOUND)
	include_directories(${SFML_INCLUDE_DIR})
	target_link_libraries(NeuralNetworkVisualizer ${SFML_LIBRARIES})
endif()

if(JSONBOX_FOUND)
	include_directories(${JSONBOX_INCLUDE_DIR})
	target_link_libraries(NeuralNetworkVisualizer ${JSONBOX_LIBRARIES})
endif()

target_link_libraries(NeuralNetworkVisualizer EvoAI)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fexpensive-optimizations")
endif()

if(NOT BUILD_STATIC AND WIN32)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -lpthread -static-libgcc -static-libstdc++ -mwindows")
endif(NOT BUILD_STATIC AND WIN32)

install(TARGETS NeuralNetworkVisualizer RUNTIME DESTINATION bin ARCHIVE DESTINATION bin)
install(DIRECTORY data DESTINATION bin)
