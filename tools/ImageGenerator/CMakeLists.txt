
set(SRCROOT ${PROJECT_SOURCE_DIR}/tools/ImageGenerator)

# all source files
set(ImageGenerator_SRC ${SRCROOT}/ImageGenerator.cpp ${SRCROOT}/imageUtils.cpp)

# define the ImageGenerator target
add_executable(ImageGenerator ${ImageGenerator_SRC})

if(SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(ImageGenerator ${SFML_LIBRARIES})
endif(SFML_FOUND)

target_link_libraries(ImageGenerator EvoAI)

install(TARGETS ImageGenerator RUNTIME DESTINATION bin ARCHIVE DESTINATION bin)