
set(SRCROOT ${PROJECT_SOURCE_DIR}/tools/NeuralNetworkVisualizer)

# all source files
set(NNVisualizer_SRC ${SRCROOT}/NeuralNetworkVisualizer.cpp)

# define the NeuralNetworkVisualizer target
add_executable(NeuralNetworkVisualizer ${NNVisualizer_SRC})
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows")
include_directories(${SFML_INCLUDE_DIR})
target_link_libraries(NeuralNetworkVisualizer ${SFML_LIBRARIES})

include_directories(${JSONBOX_INCLUDE_DIR})
target_link_libraries(NeuralNetworkVisualizer ${JSONBOX_LIBRARIES})

if(NOT BUILD_STATIC)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++ -static")
endif(NOT BUILD_STATIC)

install(TARGETS NeuralNetworkVisualizer RUNTIME DESTINATION . ARCHIVE DESTINATION .)