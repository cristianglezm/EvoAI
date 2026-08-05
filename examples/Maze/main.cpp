#include <chrono>
#include <cstdint>
#include <random>
#include <EvoAI.hpp>
#include <EvoAI/Utils.hpp>
#include "Args.hpp"
#include "LetterState.hpp"
#include "Maze.hpp"
#include "Render.hpp"
#include "Simulation.hpp"

int main(int argc, char** argv){
    enableAnsiColors();
    const ParsedArgs args = parseArgs(argc, argv);
    const unsigned seed = args.seed.value_or(static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count() & 0xFFFFFFFF));
    EvoAI::randomGen().setSeed(seed);
    const SimConfig cfg = makeConfig(args, seed);
    auto graph = buildMaze(cfg);
    clearCenter(graph, cfg);
    const auto targets = setupTargets(graph, cfg, args.text);
    const auto overlay = makeBaseOverlay(graph.size(), targets);

    std::mt19937 rngAlg(static_cast<uint32_t>(seed) + 1u);
    auto letters = makeLetterStates(args.text, cfg.startIdx, targets, rngAlg);

    const Metrics metrics = runSimulation(graph, letters, overlay);
    render(graph, overlay, letters);
    printMetrics(metrics, letters, cfg);
    return 0;
}
