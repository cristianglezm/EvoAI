
set(SRCROOT ${PROJECT_SOURCE_DIR}/tools/ImageGenerator)

# all source files
set(ImageGenerator_SRC ${SRCROOT}/ImageGenerator.cpp ${SRCROOT}/imageUtils.cpp)

# define the ImageGenerator target
add_executable(ImageGenerator ${ImageGenerator_SRC})

evoai_link_sfml(ImageGenerator)

target_link_libraries(ImageGenerator PRIVATE EvoAI)

evoai_set_target_options(ImageGenerator)

include(GNUInstallDirs)
install(TARGETS ImageGenerator RUNTIME_DEPENDENCY_SET EvoAIRuntimeDeps RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})