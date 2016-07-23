
set(SRCROOT ${PROJECT_SOURCE_DIR}/tools/ImageEvolver)

# all source files
set(ImageEvolver_SRC ${SRCROOT}/ImageEvolver.cpp)

# define the ImageEvolver target
add_executable(ImageEvolver ${ImageEvolver_SRC})

if(SFML_FOUND)
	include_directories(${SFML_INCLUDE_DIR})
	target_link_libraries(ImageEvolver ${SFML_LIBRARIES})
endif()

if(JSONBOX_FOUND)
	include_directories(${JSONBOX_INCLUDE_DIR})
	target_link_libraries(ImageEvolver ${JSONBOX_LIBRARIES})
endif()

target_link_libraries(ImageEvolver EvoAI)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fexpensive-optimizations")
endif()

if(NOT BUILD_STATIC AND WIN32)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -lpthread -static-libgcc -static-libstdc++ -mwindows")
endif(NOT BUILD_STATIC AND WIN32)

install(TARGETS ImageEvolver RUNTIME DESTINATION bin ARCHIVE DESTINATION bin)
