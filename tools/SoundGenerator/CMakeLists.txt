
set(SRCROOT ${PROJECT_SOURCE_DIR}/tools/SoundGenerator)

# all source files
set(SoundGenerator_SRC ${SRCROOT}/SoundGenerator.cpp ${SRCROOT}/soundUtils.cpp)

# define the SoundGenerator target
add_executable(SoundGenerator ${SoundGenerator_SRC})

if(SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(SoundGenerator ${SFML_LIBRARIES})
endif(SFML_FOUND)
target_link_libraries(SoundGenerator EvoAI)

install(TARGETS SoundGenerator RUNTIME DESTINATION bin ARCHIVE DESTINATION bin)