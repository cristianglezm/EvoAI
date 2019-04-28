
set(SRCROOT ${PROJECT_SOURCE_DIR}/tools/SoundGenerator)

# all source files
set(SoundGenerator_SRC ${SRCROOT}/SoundGenerator.cpp ${SRCROOT}/RawAudio.cpp ${SRCROOT}/MidiAudio.cpp)

set(SRCS
    ${SRCROOT}/midifile/Options.cpp
    ${SRCROOT}/midifile/Binasc.cpp
    ${SRCROOT}/midifile/MidiEvent.cpp
    ${SRCROOT}/midifile/MidiEventList.cpp
    ${SRCROOT}/midifile/MidiFile.cpp
    ${SRCROOT}/midifile/MidiMessage.cpp
)
set(HDRS
    ${SRCROOT}/midifile/Binasc.h
    ${SRCROOT}/midifile/MidiEvent.h
    ${SRCROOT}/midifile/MidiEventList.h
    ${SRCROOT}/midifile/MidiFile.h
    ${SRCROOT}/midifile/MidiMessage.h
    ${SRCROOT}/midifile/Options.h
)

# define the SoundGenerator target
add_executable(SoundGenerator ${SoundGenerator_SRC} ${SRCS} ${HDRS})

if(SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(SoundGenerator ${SFML_LIBRARIES})
endif(SFML_FOUND)
target_link_libraries(SoundGenerator EvoAI)

install(TARGETS SoundGenerator RUNTIME DESTINATION bin ARCHIVE DESTINATION bin)