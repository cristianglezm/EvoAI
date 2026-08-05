#pragma once
#include <optional>
#include <string>
#include <cstddef>

struct ParsedArgs{
    std::string text = "Hello, World";
    std::optional<float> randomness;
    std::optional<float> binomial;
    std::optional<unsigned> seed;
    std::optional<std::size_t> width;
    std::optional<std::size_t> height;
};

ParsedArgs parseArgs(int argc, char** argv);
void printHelp(const char* prog);
