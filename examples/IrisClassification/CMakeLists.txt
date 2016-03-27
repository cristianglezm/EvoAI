
set(SRCROOT ${PROJECT_SOURCE_DIR}/examples/IrisClassification)

# all source files
set(IrisClassification_SRC ${SRCROOT}/IrisClassification.cpp)

# define the IrisClassification target
add_executable(IrisClassification ${IrisClassification_SRC})

target_link_libraries(IrisClassification EvoAI)

install(TARGETS IrisClassification RUNTIME DESTINATION bin ARCHIVE DESTINATION bin)
install(FILES iris.data.csv DESTINATION bin)