#ifndef EVOAI_MAZE_ALGORITHMS_HPP
#define EVOAI_MAZE_ALGORITHMS_HPP

#include <vector>
#include <random>
#include <algorithm>
#include <EvoAI/Graph.hpp>
#include <EvoAI/Export.hpp>
#include <EvoAI/PathFindingAlgorithms.hpp>

/**
 * @brief Maze generation algorithms, implemented as functors decoupled from any
 *        specific Graph implementation.
 * @details Each algorithm is templated on GraphType rather than tied to EvoAI::Graph,
 *          so it can operate on any graph-like type exposing the required interface
 *          (see each functor's @tparam GraphType for the exact requirements).
 *
 * @warning Randomness is drawn from the shared EvoAI::randomGen() singleton.
 *          This follows the EvoAI convention (no engine parameter).
 *          The singleton is mutated by these algorithms and is not thread-safe.
 *          Do not call these algorithms concurrently from multiple threads.
 *          Generate mazes one at a time or from a single thread.
 */
namespace EvoAI::MazeAlgorithms{
		/**
		 * @brief Growing Tree maze generation algorithm (subtractive carver).
		 *
		 * @tparam GraphType Graph must expose Node_t and Edge_t typedefs and provide:
		 *         size(), operator[](std::size_t) -> Node_t&, forEachOutgoingEdge(const Node_t&, callback),
		 *         and isolate(const Node_t&). EvoAI::Graph satisfies this.
		 *
		 * @details This implementation is a subtractive carver: it expects @p graph to
		 *          be preconnected among carvable cells and maintains a frontier of
		 *          live cells starting at @p origin. For each frontier cell it inspects
		 *          unvisited neighbours via existing edges, keeps one neighbour as a
		 *          passage and may isolate at most one other neighbour (turning it into
		 *          a wall). Untouched neighbours remain for later resolution.
		 *
		 * @warning Randomness is drawn from the shared EvoAI::randomGen() singleton.
		 *          These algorithms mutate that singleton and are not thread-safe.
		 *          Do not call them concurrently from multiple threads; generate
		 *          mazes one at a time or from a single thread.
		 *
		 * @note This algorithm may produce loops and is not guaranteed to produce a
		 *       strict spanning tree.
		 */
        template<typename GraphType>
        struct GrowingTree{
            using Node_t = typename GraphType::Node_t;
            using Edge_t = typename GraphType::Edge_t;
			/**
			 * @brief Generates a maze in @p graph in place using the Growing Tree algorithm.
			 * @details See the class-level documentation above for the GraphType requirements,
			 *          the subtractive-carver contract, and the randomness/thread-safety warning.
			 *
			 * @param graph      GraphType& graph to carve the maze into; must already be connected.
			 * @param origin     Node_t& starting node for maze generation.
			 * @param randomness float in [0.0,1.0] bias for selecting the next frontier cell.
			 * @param binomial   float in [0.0,1.0] probability used when choosing which neighbour to carve.
			 */
            void operator()(GraphType& graph, Node_t& origin, float randomness = 0.25f, float binomial = 0.25f) noexcept;
        };
} // namespace EvoAI::MazeAlgorithms

#include "MazeAlgorithms.inl"

#endif // EVOAI_MAZE_ALGORITHMS_HPP
