#ifndef EVOAI_PATH_FINDING_ALGORITHMS_HPP
#define EVOAI_PATH_FINDING_ALGORITHMS_HPP

#include <vector>
#include <stack>
#include <queue>
#include <limits>
#include <cstdint>
#include <cstddef>
#include <cmath>
#include <algorithm>

#include <EvoAI/Graph.hpp>
#include <EvoAI/Export.hpp>

namespace EvoAI{
    /**
     *  @brief Algorithms for Path Finding Strategy in Graph.
     *  @details Algorithms compute paths between nodes given a const graph, start/end nodes,
     *  a reusable SearchContext buffer, and a TraversalPolicy that controls traversability,
     *  edge costs, and heuristics.
     */
    namespace PathFindingAlgorithms{
        /**
         * @brief Thread-local context for lock-free graph searches.
         * @details Two generation counters:
         *  - discoveredGen: stamps nodes when added to the open set; guards cost_so_far and came_from reads.
         *  - closedGen:     stamps nodes when fully expanded; guards re-expansion checks.
         * Calling reset() is a single ++generation — no memset, no fill.
         * Buffers only grow, never shrink, preserving capacity across repeated searches.
         */
        struct SearchContext{
            /**
             * @brief Prepares the context for a new search over a graph of @p size nodes.
             * @details O(1) after the initial call; buffers grow on demand.
             * @param size std::size_t number of nodes in the graph about to be searched;
             *             buffers are grown (never shrunk) to at least this size.
             */
            void reset(std::size_t size) noexcept{
                if(size > closedGen.size()){
                    closedGen.resize(size, 0);
                    discoveredGen.resize(size, 0);
                    came_from.resize(size, npos);
                    cost_so_far.resize(size, 0.0);
                }
                ++generation;
                // Handle size_t wraparound — practically unreachable (~1.8e19 searches).
                if(generation == 0){
                    ++generation;
                    std::fill(closedGen.begin(), closedGen.end(), 0);
                    std::fill(discoveredGen.begin(), discoveredGen.end(), 0);
                }
            }
            /**
             * @brief Returns true if node @p i has been added to the open set in the current search.
             * @param i std::size_t node index to query.
             * @return bool true if discovered in the current generation.
             */
            bool isDiscovered(std::size_t i) const noexcept{ return discoveredGen[i] == generation; }
            /**
             * @brief Returns true if node @p i has been fully expanded (closed set) in the current search.
             * @param i std::size_t node index to query.
             * @return bool true if closed in the current generation.
             */
            bool isClosed(std::size_t i) const noexcept{ return closedGen[i] == generation; }
            /**
             * @brief Records node @p i as discovered: sets its parent and best known cost.
             * @details Also marks the node as open. Safe to call again if a cheaper path is found;
             * this method does not itself check whether @p cost improves on a previous discovery,
             * so callers are responsible for only calling it when relaxation actually improves the cost.
             * @param i std::size_t index of the node being discovered/relaxed.
             * @param parent std::size_t index of the node it was reached from.
             * @param cost double best known accumulated cost to reach node @p i.
             */
            void discover(std::size_t i, std::size_t parent, double cost) noexcept{
                discoveredGen[i] = generation;
                came_from[i]     = parent;
                cost_so_far[i]   = cost;
            }
            /**
             * @brief Moves node @p i from open to closed (marks as fully expanded).
             * @param i std::size_t index of the node to close.
             */
            void close(std::size_t i) noexcept{ closedGen[i] = generation; }
            /**
             * @brief Returns the best known cost to reach node @p i.
             * @details Returns infinity if the node has not been discovered in the current search,
             * so relaxation comparisons work correctly without explicit resets.
             * @param i std::size_t node index to query.
             * @return double best known cost so far, or +infinity if undiscovered.
             */
            double getCost(std::size_t i) const noexcept{
                return isDiscovered(i) ? cost_so_far[i] : std::numeric_limits<double>::infinity();
            }
            /**
             * @brief Returns the parent index of node @p i in the current search tree.
             * @details Returns npos if the node has not been discovered.
             * @param i std::size_t node index to query.
             * @return std::size_t parent node index, or npos if undiscovered.
             */
            std::size_t getParent(std::size_t i) const noexcept{
                return isDiscovered(i) ? came_from[i] : npos;
            }
            // raw data — prefer the helper methods above in algorithm implementations
            std::vector<double> cost_so_far;
            std::vector<std::size_t> came_from;
            std::vector<std::size_t> closedGen;
            std::vector<std::size_t> discoveredGen;
            std::size_t generation = 1;
            static constexpr std::size_t npos = std::numeric_limits<std::size_t>::max();
        };

        /**
         * @brief Default traversal policy for Graph searches.
         * @details Adapts to NodeType capabilities at compile time via detection:
         *  - canTraverse: returns !to.blocked if NodeType has a 'blocked' field, otherwise always true.
         *  - cost:        returns static_cast<double>(edge.weight).
         *  - heuristic:   Euclidean distance if NodeType has 'x' and 'y', otherwise 0.0 (degrades A* to Dijkstra).
         * @tparam GraphType The graph type this policy operates on.
         */
        template<typename GraphType>
        struct DefaultTraversalPolicy{
            using Node_t = typename GraphType::Node_t;
            using Edge_t = typename GraphType::Edge_t;

            /**
             * @brief Checks whether the search may move from @p from to @p to across @p edge.
             * @details Compile-time branches on whether Node_t has a 'blocked' field
             * (via meta::has_blocked_v); if it does, traversal is denied into a blocked node.
             * If Node_t has no 'blocked' field, every edge is considered traversable.
             * @param from const Node_t& source node (unused unless overridden by a custom policy).
             * @param edge const Edge_t& edge being considered (unused unless overridden).
             * @param to const Node_t& destination node being tested for traversability.
             * @return bool true if the destination may be entered.
             */
            bool canTraverse(const Node_t& /*from*/, const Edge_t& /*edge*/, const Node_t& to) const noexcept{
                if constexpr(meta::has_blocked_v<Node_t>){
                    return !to.blocked;
                } else {
                    return true;
                }
            }
            /**
             * @brief Returns the cost of traversing @p edge from @p from to @p to.
             * @param from const Node_t& source node (unused; reserved for custom policies).
             * @param edge const Edge_t& edge whose weight is used as the cost.
             * @param to const Node_t& destination node (unused; reserved for custom policies).
             * @return double cost of the edge, equal to static_cast<double>(edge.weight).
             */
            double cost(const Node_t& /*from*/, const Edge_t& edge, const Node_t& /*to*/) const noexcept{
                return static_cast<double>(edge.weight);
            }
            /**
             * @brief Estimates the remaining cost from @p from to @p to.
             * @details Compile-time branches on whether Node_t has both 'x' and 'y' fields
             * (via meta::has_x_v / meta::has_y_v); if so, returns the Euclidean distance
             * between them. Otherwise returns 0.0, which degrades A* into Dijkstra's algorithm.
             * @param from const Node_t& node the estimate is computed from.
             * @param to const Node_t& target node the estimate is computed towards.
             * @return double heuristic estimate of the remaining cost to reach @p to.
             */
            double heuristic(const Node_t& from, const Node_t& to) const noexcept{
                if constexpr(meta::has_x_v<Node_t> && meta::has_y_v<Node_t>){
                    const double dx = static_cast<double>(from.x - to.x);
                    const double dy = static_cast<double>(from.y - to.y);
                    return std::sqrt(dx * dx + dy * dy);
                } else {
                    return 0.0;
                }
            }
        };

        /**
         * @brief Depth-First Search algorithm.
         * @details Locates a path to destination by travelling as deep as possible first.
         * Uses Policy::canTraverse to decide which edges may be followed; does not
         * consult Policy::cost or Policy::heuristic, so found paths are not guaranteed
         * to be shortest or cheapest.
         * @tparam GraphType Graph to search. Must expose Node_t/Edge_t typedefs and
         *         a const-compatible read interface (size(), operator[](index) const,
         *         forEachOutgoingEdge(node, callback) const) — satisfied by both
         *         EvoAI::Graph and EvoAI::LatticeGraph.
         * @tparam Policy Traversal policy; defaults to DefaultTraversalPolicy<GraphType>.
         *         Must provide canTraverse(from, edge, to) -> bool; see DefaultTraversalPolicy.
         */
        template<typename GraphType, typename Policy = DefaultTraversalPolicy<GraphType>>
        struct DFS{
            using Node_t = typename GraphType::Node_t;
            using Edge_t = typename GraphType::Edge_t;
            static_assert(meta::has_can_traverse_v<Policy, Node_t, Edge_t>,
                "Policy must provide bool canTraverse(const Node_t&, const Edge_t&, const Node_t&) const noexcept; DFS does not use cost or heuristic.");
            /**
             * @brief Computes a path from @p start to @p end using DFS.
             * @param graph const reference to the graph (not modified during search).
             * @param start The starting node.
             * @param end The destination node.
             * @param ctx Reusable SearchContext (reset internally before use).
             * @param policy Traversal policy instance.
             * @return Path from start to end as const node pointers, or empty if not found.
             */
            std::vector<const Node_t*> operator()(const GraphType& graph, const Node_t& start, const Node_t& end,
                                                   SearchContext& ctx, Policy policy = Policy{}) noexcept;
        };

        /**
         * @brief Breadth-First Search algorithm.
         * @details Locates the shortest (unweighted) path by expanding nearest neighbours first.
         * Uses Policy::canTraverse to decide which edges may be followed; treats all
         * traversable edges as equal cost (does not consult Policy::cost or Policy::heuristic),
         * so the path found is shortest by hop count, not necessarily by edge weight.
         * @tparam GraphType Graph to search. Must expose Node_t/Edge_t typedefs and
         *         a const-compatible read interface (size(), operator[](index) const,
         *         forEachOutgoingEdge(node, callback) const) — satisfied by both
         *         EvoAI::Graph and EvoAI::LatticeGraph.
         * @tparam Policy Traversal policy; defaults to DefaultTraversalPolicy<GraphType>.
         *         Must provide canTraverse(from, edge, to) -> bool; see DefaultTraversalPolicy.
         */
        template<typename GraphType, typename Policy = DefaultTraversalPolicy<GraphType>>
        struct BFS{
            using Node_t = typename GraphType::Node_t;
            using Edge_t = typename GraphType::Edge_t;
            static_assert(meta::has_can_traverse_v<Policy, Node_t, Edge_t>,
                "Policy must provide bool canTraverse(const Node_t&, const Edge_t&, const Node_t&) const noexcept; BFS does not use cost or heuristic.");
            /**
             * @brief Computes the shortest path from @p start to @p end using BFS.
             * @param graph const reference to the graph (not modified during search).
             * @param start The starting node.
             * @param end The destination node.
             * @param ctx Reusable SearchContext (reset internally before use).
             * @param policy Traversal policy instance.
             * @return Shortest path from start to end as const node pointers, or empty if not found.
             */
            std::vector<const Node_t*> operator()(const GraphType& graph, const Node_t& start, const Node_t& end,
                                                   SearchContext& ctx, Policy policy = Policy{}) noexcept;
        };

        /**
         * @brief Greedy Best-First Search algorithm.
         * @details Expands the node closest to the goal according to the policy heuristic.
         * Not guaranteed to find the shortest path.
         * Uses Policy::canTraverse to decide which edges may be followed and
         * Policy::heuristic to rank candidates; does not accumulate Policy::cost.
         * @tparam GraphType Graph to search. Must expose Node_t/Edge_t typedefs and
         *         a const-compatible read interface (size(), operator[](index) const,
         *         forEachOutgoingEdge(node, callback) const) — satisfied by both
         *         EvoAI::Graph and EvoAI::LatticeGraph.
         * @tparam Policy Traversal policy; defaults to DefaultTraversalPolicy<GraphType>.
         *         Must provide canTraverse(from, edge, to) -> bool and
         *         heuristic(from, to) -> double; see DefaultTraversalPolicy.
         */
        template<typename GraphType, typename Policy = DefaultTraversalPolicy<GraphType>>
        struct GBFS{
            using Node_t = typename GraphType::Node_t;
            using Edge_t = typename GraphType::Edge_t;
            static_assert(meta::has_can_traverse_v<Policy, Node_t, Edge_t>,
                "Policy must provide bool canTraverse(const Node_t&, const Edge_t&, const Node_t&) const noexcept.");
            static_assert(meta::has_heuristic_v<Policy, Node_t>,
                "Policy must provide double heuristic(const Node_t&, const Node_t&) const noexcept; GBFS ranks purely on heuristic and does not use cost.");
            /**
             * @brief Computes a path from @p start to @p end using GBFS.
             * @param graph const reference to the graph (not modified during search).
             * @param start The starting node.
             * @param end The destination node.
             * @param ctx Reusable SearchContext (reset internally before use).
             * @param policy Traversal policy instance.
             * @return Path from start to end as const node pointers, or empty if not found.
             */
            std::vector<const Node_t*> operator()(const GraphType& graph, const Node_t& start, const Node_t& end,
                                                   SearchContext& ctx, Policy policy = Policy{}) noexcept;
        };

        /**
         * @brief A* Search algorithm.
         * @details Combines actual edge cost with a heuristic estimate.
         * With DefaultTraversalPolicy: uses Euclidean heuristic if NodeType has 'x'/'y',
         * otherwise degrades to Dijkstra (heuristic = 0).
         * Uses Policy::canTraverse to decide which edges may be followed,
         * Policy::cost to accumulate the real path cost, and Policy::heuristic to
         * guide expansion order; finds the optimal path when the heuristic is admissible.
         * @tparam GraphType Graph to search. Must expose Node_t/Edge_t typedefs and
         *         a const-compatible read interface (size(), operator[](index) const,
         *         forEachOutgoingEdge(node, callback) const) — satisfied by both
         *         EvoAI::Graph and EvoAI::LatticeGraph.
         * @tparam Policy Traversal policy; defaults to DefaultTraversalPolicy<GraphType>.
         *         Must provide canTraverse(from, edge, to) -> bool, cost(from, edge, to) -> double,
         *         and heuristic(from, to) -> double; see DefaultTraversalPolicy.
         */
        template<typename GraphType, typename Policy = DefaultTraversalPolicy<GraphType>>
        struct AStar{
            using Node_t = typename GraphType::Node_t;
            using Edge_t = typename GraphType::Edge_t;
            static_assert(meta::has_can_traverse_v<Policy, Node_t, Edge_t>,
                "Policy must provide bool canTraverse(const Node_t&, const Edge_t&, const Node_t&) const noexcept.");
            static_assert(meta::has_cost_v<Policy, Node_t, Edge_t>,
                "Policy must provide double cost(const Node_t&, const Edge_t&, const Node_t&) const noexcept.");
            static_assert(meta::has_heuristic_v<Policy, Node_t>,
                "Policy must provide double heuristic(const Node_t&, const Node_t&) const noexcept.");
            /**
             * @brief Computes the optimal path from @p start to @p end using A*.
             * @param graph const reference to the graph (not modified during search).
             * @param start The starting node.
             * @param end The destination node.
             * @param ctx Reusable SearchContext (reset internally before use).
             * @param policy Traversal policy instance.
             * @return Optimal path from start to end as const node pointers, or empty if not found.
             */
            std::vector<const Node_t*> operator()(const GraphType& graph, const Node_t& start, const Node_t& end,
                                                   SearchContext& ctx, Policy policy = Policy{}) noexcept;
        };

    } // namespace PathFindingAlgorithms
} // namespace EvoAI

#include "PathFindingAlgorithms.inl"

#endif // EVOAI_PATH_FINDING_ALGORITHMS_HPP
