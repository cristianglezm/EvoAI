
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
    include_directories(SoundGenerator PRIVATE ${SFML_INCLUDE_DIR})
    target_link_libraries(SoundGenerator PRIVATE ${SFML_LIBRARIES})
endif(SFML_FOUND)
target_link_libraries(SoundGenerator PRIVATE EvoAI)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    message(STATUS "SoundGenerator - Compiler gcc")
    target_compile_options(SoundGenerator PRIVATE -std=c++17 -Wall -Wextra -Wshadow)
    if(EvoAI_BUILD_STATIC)
        target_link_options(SoundGenerator PRIVATE -static -static-libgcc -static-libstdc++)
    endif()
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(SoundGenerator PRIVATE -O3 -fexpensive-optimizations -DNDEBUG)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(SoundGenerator PRIVATE -g)
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(STATUS "SoundGenerator - Compiler clang")
    target_compile_options(SoundGenerator PRIVATE -std=c++17 -Wall -Wextra -Wshadow)
    if(EvoAI_BUILD_STATIC)
        if(NOT APPLE)
            target_link_options(SoundGenerator PRIVATE -static -static-libgcc -static-libstdc++)
        endif()
    endif()
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(SoundGenerator PRIVATE -O3 -DNDEBUG)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(SoundGenerator PRIVATE -g)
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    message(STATUS "SoundGenerator - Compiler MSVC")
    target_compile_options(SoundGenerator PRIVATE /std:c++17 /W4)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(SoundGenerator PRIVATE /O3 /DNDEBUG)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(SoundGenerator PRIVATE /g)
    endif()
else()
    message(WARNING "SoundGenerator - Compiler not supported.")
endif()

include(GNUInstallDirs)
install(TARGETS SoundGenerator 
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})