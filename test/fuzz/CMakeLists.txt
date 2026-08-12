# libFuzzer harnesses for EvoAI's JSON-deserialization constructors.
# Only built when EvoAI_BUILD_FUZZERS is ON, See README.md for what's
# covered and how to run these locally.

set(EvoAI_FUZZ_TARGETS
    fuzz_jsonbox_parse
    fuzz_neuralnetwork_from_json
    fuzz_genome_from_json
    fuzz_graph_from_json
)

foreach(fuzzTarget ${EvoAI_FUZZ_TARGETS})
    add_executable(${fuzzTarget} ${fuzzTarget}.cpp)
    target_compile_options(${fuzzTarget} PRIVATE -fsanitize=fuzzer,address,undefined -g -O1)
    target_link_options(${fuzzTarget} PRIVATE -fsanitize=fuzzer,address,undefined)
    target_link_libraries(${fuzzTarget} PRIVATE EvoAI)
    set_target_properties(${fuzzTarget} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/fuzz/bin"
    )
endforeach()
