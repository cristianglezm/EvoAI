
set(SRCROOT ${PROJECT_SOURCE_DIR}/test/NeuralNetworkVisualizer)

# all source files
set(SRC ${SRCROOT}/NeuralNetworkVisualizer.cpp)

if(SFML_FOUND)
	include_directories(${SFML_INCLUDE_DIR})
	target_link_libraries(NeuralNetworkVisualizer ${SFML_LIBRARIES})
endif(SFML_FOUND)

if(JSONBOX_FOUND)
	include_directories(${JSONBOX_INCLUDE_DIR})
	target_link_libraries(NeuralNetworkVisualizer ${JSONBOX_LIBRARIES})
endif(JSONBOX_FOUND)

# define the NeuralNetworkVisualizer target
add_executable(NeuralNetworkVisualizer ${SRC})