
set(SRCROOT ${PROJECT_SOURCE_DIR}/examples/NumClassifier)

# all source files
set(NumClassifier_SRC ${SRCROOT}/NumClassifier.cpp)

# define the NumClassifier target
add_executable(NumClassifier ${NumClassifier_SRC})

target_link_libraries(NumClassifier PRIVATE EvoAI)

evoai_set_target_options(NumClassifier)

include(GNUInstallDirs)
install(TARGETS NumClassifier RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})