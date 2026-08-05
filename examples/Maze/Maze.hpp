#pragma once
#include <string>
#include <vector>
#include <EvoAI.hpp>
#include "Args.hpp"
#include "MazeNode.hpp"

using GraphT = EvoAI::LatticeGraph<MazeNode>;

struct SimConfig{
    float randomness;
    float binomial;
    unsigned int seed;
    std::size_t cols, rows;
    std::size_t centerRow, centerCol;
    std::size_t startIdx;
    int clearRX, clearRY;
};

SimConfig makeConfig(const ParsedArgs& args, unsigned int seed);
GraphT buildMaze(const SimConfig& cfg);
void clearCenter(GraphT& g, const SimConfig& cfg);
std::vector<std::size_t> setupTargets(GraphT& g, const SimConfig& cfg, const std::string& text);
std::vector<char> makeBaseOverlay(std::size_t size, const std::vector<std::size_t>& targets);
