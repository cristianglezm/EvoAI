#include "Maze.hpp"
#include <algorithm>
#include <EvoAI/MazeAlgorithms.hpp>
#include <EvoAI/Utils.hpp>

namespace MazeAlg = EvoAI::MazeAlgorithms;

SimConfig makeConfig(const ParsedArgs& args, unsigned int seed){
    SimConfig cfg;
    cfg.seed = seed;
    cfg.randomness = args.randomness.value_or(EvoAI::randomGen().random(0.0f, 1.0f));
    cfg.binomial = args.binomial.value_or(  EvoAI::randomGen().random(0.0f, 1.0f));
    cfg.rows = args.height.value_or(std::size_t{25});
    cfg.cols = args.width.value_or(std::max(std::size_t{25}, args.text.size() + 6));
    cfg.centerRow = cfg.rows / 2;
    cfg.centerCol = cfg.cols / 2;
    cfg.startIdx = cfg.centerRow * cfg.cols + cfg.centerCol;
    cfg.clearRX = 3;
    cfg.clearRY = 1;
    return cfg;
}

GraphT buildMaze(const SimConfig& cfg){
    GraphT g(cfg.rows, cfg.cols, false);
    for(std::size_t i = 0; i < g.size(); ++i){
        g[i].x = static_cast<int>(i % cfg.cols);
        g[i].y = static_cast<int>(i / cfg.cols);
    }
    MazeAlg::GrowingTree<GraphT> growingTree;
    growingTree(g, g[cfg.startIdx], cfg.randomness, cfg.binomial);
    return g;
}

void clearCenter(GraphT& g, const SimConfig& cfg){
    const int rows = static_cast<int>(cfg.rows);
    const int cols = static_cast<int>(cfg.cols);
    for(int dy = -cfg.clearRY; dy <= cfg.clearRY; ++dy){
        for(int dx = -cfg.clearRX; dx <= cfg.clearRX; ++dx){
            int r = static_cast<int>(cfg.centerRow) + dy;
            int c = static_cast<int>(cfg.centerCol) + dx;
            if(r < 0 || r >= rows || c < 0 || c >= cols){
				continue;
			}
            // LatticeGraph has no connectToX()/connect() -- forEachOutgoingEdge derives
            // edges from .blocked + index math on every call, so unblocking is enough;
            // connectivity to any already-open neighbour comes back automatically.
            std::size_t idx = static_cast<std::size_t>(r) * cfg.cols + static_cast<std::size_t>(c);
            g[idx].blocked = false;
        }
    }
}

std::vector<std::size_t> setupTargets(GraphT& g, const SimConfig& cfg, const std::string& text){
    const int cols      = static_cast<int>(cfg.cols);
    const int bottomRow = static_cast<int>(cfg.rows) - 1;
    int firstCol = static_cast<int>(cfg.centerCol) - static_cast<int>(text.size()) / 2;
    if(firstCol < 1){
		firstCol = 1;
	}
    std::vector<std::size_t> targets;
    targets.reserve(text.size());
    for(std::size_t i = 0; i < text.size(); ++i){
        int c = firstCol + static_cast<int>(i);
        if(c >= cols - 1){
			c = cols - 2;
		}
        std::size_t idx = static_cast<std::size_t>(bottomRow) * cfg.cols + static_cast<std::size_t>(c);
        g[idx].blocked = false;
        // Also open the cell directly above so a search can actually arrive into the
        // target from the rest of the maze; forEachOutgoingEdge handles both directions
        // of that connection on its own once neither endpoint is blocked.
        if(bottomRow > 0){
            std::size_t northIdx = static_cast<std::size_t>(bottomRow - 1) * cfg.cols + static_cast<std::size_t>(c);
            g[northIdx].blocked = false;
        }
        targets.push_back(idx);
    }
    return targets;
}

std::vector<char> makeBaseOverlay(std::size_t size, const std::vector<std::size_t>& targets){
    std::vector<char> overlay(size, '\0');
    for(auto idx : targets){
		overlay[idx] = '_';
	}
    return overlay;
}
