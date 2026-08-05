#pragma once
#include <memory>
#include <vector>
#include "LetterState.hpp"
#include "Maze.hpp"

struct Metrics{
    void record(long long ns, std::size_t len) {
        timesNs.push_back(ns); pathLengths.push_back(len);
    }
	// data
    std::vector<long long>   timesNs;
    std::vector<std::size_t> pathLengths;
};

Metrics runSimulation(const GraphT& graph,
                      std::vector<std::unique_ptr<LetterState>>& letters,
                      const std::vector<char>& overlay);

void printMetrics(const Metrics& m,
                  const std::vector<std::unique_ptr<LetterState>>& letters,
                  const SimConfig& cfg);
