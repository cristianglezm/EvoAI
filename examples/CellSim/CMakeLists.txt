
set(SRCROOT ${PROJECT_SOURCE_DIR}/examples/CellSim)

# define the CellSim target
add_executable(CellSim)

target_sources(CellSim
	PUBLIC
		${SRCROOT}/Tournament.hpp
		${SRCROOT}/CellSim.hpp
		${SRCROOT}/Cell.hpp
		${SRCROOT}/InfoBar.hpp
		${SRCROOT}/Options.hpp
		${SRCROOT}/Averages.hpp
		${SRCROOT}/Timer.hpp
		${SRCROOT}/Trainer.hpp
	PRIVATE
		${SRCROOT}/CellSim.cpp
		${SRCROOT}/Cell.cpp
		${SRCROOT}/InfoBar.cpp
		${SRCROOT}/Options.cpp
		${SRCROOT}/Averages.cpp
		${SRCROOT}/Timer.cpp
		${SRCROOT}/Trainer.cpp
)

evoai_link_sfml(CellSim)

if(JsonBox_FOUND)
	target_include_directories(CellSim PRIVATE ${JsonBox_INCLUDE_DIR})
	target_link_libraries(CellSim PRIVATE ${JsonBox_LIBRARY})
endif()

target_link_libraries(CellSim PRIVATE EvoAI)

evoai_set_target_options(CellSim WIN32_GUI)

include(GNUInstallDirs)
install(TARGETS CellSim RUNTIME_DEPENDENCY_SET EvoAIRuntimeDeps RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
