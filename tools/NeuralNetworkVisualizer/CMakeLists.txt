
set(SRCROOT ${PROJECT_SOURCE_DIR}/tools/NeuralNetworkVisualizer)

# all source files
set(NNVisualizer_SRC ${SRCROOT}/NeuralNetworkVisualizer.cpp ${SRCROOT}/NNRenderer.cpp ${SRCROOT}/NNRenderer.hpp)

# define the NeuralNetworkVisualizer target
add_executable(NeuralNetworkVisualizer ${NNVisualizer_SRC})

evoai_link_sfml(NeuralNetworkVisualizer)

if(JsonBox_FOUND)
	target_include_directories(NeuralNetworkVisualizer PRIVATE ${JsonBox_INCLUDE_DIR})
	target_link_libraries(NeuralNetworkVisualizer PRIVATE ${JsonBox_LIBRARY})
endif()

target_link_libraries(NeuralNetworkVisualizer PRIVATE EvoAI)

evoai_set_target_options(NeuralNetworkVisualizer WIN32_GUI)

# copy file to "build"
FILE(COPY data DESTINATION ${CMAKE_BINARY_DIR}/bin/EvoAI/.)
include(GNUInstallDirs)
install(TARGETS NeuralNetworkVisualizer 
        RUNTIME_DEPENDENCY_SET EvoAIRuntimeDeps
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY data DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/EvoAI/)
