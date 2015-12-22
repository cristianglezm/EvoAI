
set(SRCROOT ${PROJECT_SOURCE_DIR}/examples/XOR)

# all source files
set(XOR_SRC ${SRCROOT}/XOR.cpp)

# define the XOR target
add_executable(XOR ${XOR_SRC})

target_link_libraries(XOR EvoAI)

install(TARGETS XOR RUNTIME DESTINATION . ARCHIVE DESTINATION .)