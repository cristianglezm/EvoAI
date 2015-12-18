
set(SRCROOT ${PROJECT_SOURCE_DIR}/test/XOR)

# all source files
set(SRC ${SRCROOT}/XOR.cpp)

# define the XOR target
add_executable(XOR SOURCES ${SRC} DEPENDS SFML)
