#pragma once
#include <memory>
#include <mutex>
#include <vector>
#include "LetterState.hpp"
#include "Maze.hpp"
#include <EvoAI.hpp>

extern std::mutex drawMutex; // defined in Render.cpp, shared with Simulation.cpp

void enableAnsiColors();
void resetCursor();
void render(const GraphT& g,
            const std::vector<char>& overlay,
            const std::vector<std::unique_ptr<LetterState>>& letters);
