
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

evoai_link_sfml(SoundGenerator)
target_link_libraries(SoundGenerator PRIVATE EvoAI)

evoai_set_target_options(SoundGenerator)

include(GNUInstallDirs)
install(TARGETS SoundGenerator 
        RUNTIME_DEPENDENCY_SET EvoAIRuntimeDeps
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})