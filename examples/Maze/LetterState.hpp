#pragma once
#include <atomic>
#include <memory>
#include <random>
#include <string>
#include <vector>
#include "Ansi.hpp"

enum class Algorithm { DFS, BFS, GBFS, ASTAR };

inline Algorithm pickRandomAlgorithm(std::mt19937& rng){
    return static_cast<Algorithm>(std::uniform_int_distribution<int>(0, 3)(rng));
}
inline const char* algoName(Algorithm a){
    switch(a){
        case Algorithm::DFS:   return "DFS";
        case Algorithm::BFS:   return "BFS";
        case Algorithm::GBFS:  return "GBFS";
        case Algorithm::ASTAR: return "A*";
    }
    return "?";
}
inline const char* algoColor(Algorithm a){
    switch(a){
        case Algorithm::DFS: return CYAN;
        case Algorithm::BFS: return GREEN;
        case Algorithm::GBFS: return YELLOW;
        case Algorithm::ASTAR: return MAGENTA;
    }
    return WHITE;
}

struct LetterState{
    LetterState() = default;
    LetterState(const LetterState&) = delete;
    LetterState& operator=(const LetterState&) = delete;
	// data
    char ch = '\0';
    std::atomic<std::size_t> currentIdx{0};
    std::size_t goalIdx  = 0;
    Algorithm algo = Algorithm::ASTAR;
    std::atomic<bool> finished{false};
};

inline std::vector<std::unique_ptr<LetterState>> makeLetterStates(const std::string& text, std::size_t startIdx, 
																	const std::vector<std::size_t>& targets, std::mt19937& rng){
	std::vector<std::unique_ptr<LetterState>> letters;
    letters.reserve(text.size());
    for(std::size_t i = 0; i < text.size(); ++i){
        auto ls = std::make_unique<LetterState>();
        ls->ch  = text[i];
        ls->currentIdx.store(startIdx);
        ls->goalIdx = targets[i % targets.size()];
        ls->algo = pickRandomAlgorithm(rng);
        ls->finished.store(false);
        letters.push_back(std::move(ls));
    }
    return letters;
}
