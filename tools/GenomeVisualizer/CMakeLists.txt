
set(SRCROOT ${PROJECT_SOURCE_DIR}/tools/GenomeVisualizer)

# all source files
set(GVisualizer_SRC ${SRCROOT}/GenomeVisualizer.cpp ${SRCROOT}/GRenderer.cpp ${SRCROOT}/GRenderer.hpp)

# define the GenomeVisualizer target
add_executable(GenomeVisualizer ${GVisualizer_SRC})

if(SFML_FOUND)
	include_directories(${SFML_INCLUDE_DIR})
	target_link_libraries(GenomeVisualizer ${SFML_LIBRARIES})
endif()

if(JSONBOX_FOUND)
	include_directories(${JSONBOX_INCLUDE_DIR})
	target_link_libraries(GenomeVisualizer ${JSONBOX_LIBRARIES})
endif()

target_link_libraries(GenomeVisualizer EvoAI)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fexpensive-optimizations")
endif()

if(NOT BUILD_STATIC AND WIN32)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -lpthread -static-libgcc -static-libstdc++ -mwindows")
endif(NOT BUILD_STATIC AND WIN32)

install(TARGETS GenomeVisualizer RUNTIME DESTINATION bin ARCHIVE DESTINATION bin)
