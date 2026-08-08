# Usage: evoai_set_target_options(<target> [THREADS] [WIN32_GUI])
#   THREADS   - links Threads::Threads (needed when target uses std::thread directly)
#   WIN32_GUI - suppresses the console window (Windows only)

function(evoai_set_target_options TARGET)
    cmake_parse_arguments(ARG "THREADS;WIN32_GUI" "" "" ${ARGN})

    # CXX_STANDARD_REQUIRED forces -std=c++17 rather than a floor (GCC 16 defaults to C++23)
    target_compile_features(${TARGET} PRIVATE cxx_std_17)
    set_target_properties(${TARGET} PROPERTIES
        CXX_STANDARD          17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS        OFF
    )

    target_compile_options(${TARGET} PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wshadow>
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
    )

    target_compile_options(${TARGET} PRIVATE
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:Release>>:-O3 -fexpensive-optimizations>
        $<$<AND:$<CXX_COMPILER_ID:Clang,AppleClang>,$<CONFIG:Release>>:-O3>
        $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/O2>
        $<$<AND:$<CXX_COMPILER_ID:GNU,Clang,AppleClang>,$<CONFIG:Debug>>:-g>
        $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/Zi>
    )

    target_compile_definitions(${TARGET} PRIVATE
        $<$<CONFIG:Release>:NDEBUG>
    )

    if(EvoAI_BUILD_STATIC)
        target_link_options(${TARGET} PRIVATE
            $<$<CXX_COMPILER_ID:GNU>:-static -static-libgcc -static-libstdc++>
            $<$<AND:$<CXX_COMPILER_ID:Clang>,$<NOT:$<PLATFORM_ID:Darwin>>>:-static -static-libgcc -static-libstdc++>
        )
    endif()

    if(ARG_THREADS)
        find_package(Threads REQUIRED)
        target_link_libraries(${TARGET} PRIVATE Threads::Threads)
    endif()

    # -mwindows is Windows-only; guard it or it breaks Linux/macOS
    if(ARG_WIN32_GUI)
        target_link_options(${TARGET} PRIVATE
            $<$<AND:$<CXX_COMPILER_ID:GNU,Clang>,$<PLATFORM_ID:Windows>>:-mwindows>
        )
        if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
            set_target_properties(${TARGET} PROPERTIES WIN32_EXECUTABLE TRUE)
        endif()
    endif()
endfunction()

# Links SFML against a target, whichever way it was provided:
#   - real imported targets (sfml-graphics etc.) from SFML's own Config mode or FetchContent
#   - this project's legacy FindSFML.cmake Module (SFML_INCLUDE_DIR/SFML_LIBRARIES variables)
# Module mode wins the find_package() search whenever cmake/modules/FindSFML.cmake is on
# CMAKE_MODULE_PATH, so the variable path is what's normally hit; the target path covers
# EvoAI_BUILD_STATIC's FetchContent fallback, which only ever produces real targets.
function(evoai_link_sfml TARGET)
    if(TARGET sfml-graphics)
        target_link_libraries(${TARGET} PRIVATE sfml-graphics sfml-window sfml-system sfml-audio)
    else()
        target_include_directories(${TARGET} PRIVATE ${SFML_INCLUDE_DIR})
        target_link_libraries(${TARGET} PRIVATE ${SFML_LIBRARIES})
    endif()
endfunction()
