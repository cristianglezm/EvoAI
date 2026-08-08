
set(SRCROOT ${PROJECT_SOURCE_DIR}/examples/IrisClassification)

# all source files
set(IrisClassification_SRC ${SRCROOT}/IrisClassification.cpp)

# define the IrisClassification target
add_executable(IrisClassification ${IrisClassification_SRC})

target_link_libraries(IrisClassification PRIVATE EvoAI)

evoai_set_target_options(IrisClassification)

# copy to "build"
FILE(COPY_FILE iris.data.csv ${CMAKE_BINARY_DIR}/iris.data.csv ONLY_IF_DIFFERENT)
include(GNUInstallDirs)
install(TARGETS IrisClassification RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES iris.data.csv DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/EvoAI/data/)
