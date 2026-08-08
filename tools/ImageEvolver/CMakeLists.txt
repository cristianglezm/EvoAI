
set(SRCROOT ${PROJECT_SOURCE_DIR}/tools/ImageEvolver)

# all source files
set(ImageEvolver_SRC ${SRCROOT}/ImageEvolver.cpp)

# define the ImageEvolver target
add_executable(ImageEvolver ${ImageEvolver_SRC})

evoai_link_sfml(ImageEvolver)

if(JsonBox_FOUND)
	target_include_directories(ImageEvolver PRIVATE ${JsonBox_INCLUDE_DIR})
	target_link_libraries(ImageEvolver PRIVATE ${JsonBox_LIBRARY})
endif()

target_link_libraries(ImageEvolver PRIVATE EvoAI)

evoai_set_target_options(ImageEvolver WIN32_GUI)

include(GNUInstallDirs)
install(TARGETS ImageEvolver RUNTIME_DEPENDENCY_SET EvoAIRuntimeDeps RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
