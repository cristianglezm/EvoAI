
set(SRCROOT ${PROJECT_SOURCE_DIR}/tools/GenomeVisualizer)

# all source files
set(GVisualizer_SRC ${SRCROOT}/GenomeVisualizer.cpp ${SRCROOT}/GRenderer.cpp ${SRCROOT}/GRenderer.hpp)

# define the GenomeVisualizer target
add_executable(GenomeVisualizer ${GVisualizer_SRC})

evoai_link_sfml(GenomeVisualizer)

if(JsonBox_FOUND)
	target_include_directories( GenomeVisualizer PRIVATE ${JsonBox_INCLUDE_DIR})
	target_link_libraries(GenomeVisualizer PRIVATE ${JsonBox_LIBRARY})
endif()

target_link_libraries(GenomeVisualizer PRIVATE EvoAI)

evoai_set_target_options(GenomeVisualizer WIN32_GUI)

include(GNUInstallDirs)
install(TARGETS GenomeVisualizer RUNTIME_DEPENDENCY_SET EvoAIRuntimeDeps RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
