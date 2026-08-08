
set(SRCROOT ${PROJECT_SOURCE_DIR}/tools/ImageMixer)

# all source files
set(ImageMixer_SRC ${SRCROOT}/ImageMixer.cpp)

# define the ImageMixer target
add_executable(ImageMixer ${ImageMixer_SRC})

evoai_link_sfml(ImageMixer)

target_link_libraries(ImageMixer PRIVATE EvoAI)

evoai_set_target_options(ImageMixer)

include(GNUInstallDirs)
install(TARGETS ImageMixer RUNTIME_DEPENDENCY_SET EvoAIRuntimeDeps RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})