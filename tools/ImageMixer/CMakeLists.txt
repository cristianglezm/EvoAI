
set(SRCROOT ${PROJECT_SOURCE_DIR}/tools/ImageMixer)

# all source files
set(ImageMixer_SRC ${SRCROOT}/ImageMixer.cpp)

# define the ImageMixer target
add_executable(ImageMixer ${ImageMixer_SRC})

if(SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(ImageMixer ${SFML_LIBRARIES})
endif(SFML_FOUND)

target_link_libraries(ImageMixer EvoAI)

install(TARGETS ImageMixer RUNTIME DESTINATION bin ARCHIVE DESTINATION bin)