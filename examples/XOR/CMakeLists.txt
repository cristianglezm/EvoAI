
set(SRCROOT ${PROJECT_SOURCE_DIR}/examples/XOR)

# all source files
set(XOR_SRC ${SRCROOT}/XOR.cpp)

# define the XOR target
add_executable(XOR ${XOR_SRC})

target_link_libraries(XOR PRIVATE EvoAI)

evoai_set_target_options(XOR)

include(GNUInstallDirs)
install(TARGETS XOR RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})