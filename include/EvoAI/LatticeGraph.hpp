#ifndef EVOAI_LATTICE_GRAPH_HPP
#define EVOAI_LATTICE_GRAPH_HPP
#include <vector>
#include <cstddef>
#include <stdexcept>
#include <EvoAI/Utils/TypeUtils.hpp>
#include <EvoAI/Graph.hpp>

namespace EvoAI{
    /**
     * @brief Implicit lattice (grid) graph that computes edges on the fly instead of storing them.
     *
     * @tparam NodeType User-supplied node type. Must satisfy meta::is_a_graph_node_v
     *         (toJson(), a constructor accepting JsonBox::Object, and an integral index
     *         field) AND meta::has_blocked_v (a 'blocked' field), the latter being what
     *         backs isolate() - see below. Defaults to NodeDefault.
     * @tparam EdgeType User-supplied edge type. Must satisfy meta::is_a_graph_edge_v,
     *         which requires: a toJson() method, a constructor accepting JsonBox::Object,
     *         and integral fields prev, next, and weight. Defaults to EdgeDefault.
     * @details Unlike EvoAI::Graph, this class holds no edge container: connectivity is
     *          derived from index math on every forEachOutgoingEdge call, trading arbitrary
     *          topology for zero storage overhead on graphs that are always a regular,
     *          fixed-shape 4- or 8-neighbour lattice.
     *
     *          isolate() is backed by NodeType's existing blocked field rather than separate
     *          per-node state - deliberately: it's the same field
     *          PathFindingAlgorithms::DefaultTraversalPolicy::canTraverse and
     *          EvoAI::MazeAlgorithms::GrowingTree already expect, so both work on a
     *          LatticeGraph exactly as they do on a Graph. See forEachOutgoingEdge() and
     *          isolate() below for the exact mechanics.
     */
    template<typename NodeType = NodeDefault, typename EdgeType = EdgeDefault>
    class LatticeGraph {
        static_assert(meta::is_a_graph_node_v<NodeType>, "NodeType must satisfy meta::is_a_graph_node_v.");
        static_assert(meta::is_a_graph_edge_v<EdgeType>, "EdgeType must satisfy meta::is_a_graph_edge_v.");
        static_assert(meta::has_blocked_v<NodeType>, "NodeType must have a 'blocked' field; LatticeGraph uses it to back isolate().");
        public:
            using Node_t = NodeType;
            using Edge_t = EdgeType;
        public:
            /**
             * @brief Creates a row * col lattice graph with sequentially indexed nodes.
             * @param r std::size_t number of rows in the grid
             * @param c std::size_t number of columns in the grid
             * @param diag bool if true, each node is also connected to its 4 diagonal
             *             neighbours in addition to the 4 cardinal ones (default false)
             */
            explicit LatticeGraph(std::size_t r, std::size_t c, bool diag = false)
            : row(r)
            , col(c)
            , diagonal(diag)
            , nodes(r * c){
                for(std::size_t i = 0; i < nodes.size(); ++i){
                    nodes[i].index = i;
                }
            }
            /**
             * @brief Returns the total number of nodes in the graph (row * col).
             * @return std::size_t number of nodes
             */
            std::size_t size() const noexcept{
                return nodes.size();
            }
            /**
             * @brief Returns the number of rows in the grid layout.
             * @return std::size_t current row count
             */
            std::size_t getRow() const noexcept{
                return row;
            }
            /**
             * @brief Returns the number of columns in the grid layout.
             * @return std::size_t current column count
             */
            std::size_t getCol() const noexcept{
                return col;
            }
            /**
             * @brief Returns a reference to the node at the given flat index.
             * @param index std::size_t flat node index
             * @return Node_t& reference to the node
             */
            Node_t& operator[](std::size_t index){
                return nodes[index];
            }
            /**
             * @brief Returns a const reference to the node at the given flat index.
             * @param index std::size_t flat node index
             * @return const Node_t& const reference to the node
             */
            const Node_t& operator[](std::size_t index) const{
                return nodes[index];
            }
            /**
              * @brief Iterates over all outgoing edges of @p n without allocating.
              *
              * Derives @p n's grid coordinates from its index, then synthesizes an
              * Edge_t for each cardinal neighbour that exists (weight 1), and additionally
              * for each diagonal neighbour when the graph was constructed with diagonal = true
              * (weight 2). Out-of-bounds neighbours (grid edges/corners) are silently skipped,
              * as are any candidate edges touching a blocked node: if @p n itself is blocked
              * this yields nothing, and any neighbour that is blocked is skipped as a candidate
              * regardless of @p n's own state.
              *
              * @note Diagonal weight is 2, not 1: real diagonal distance is sqrt(2) ~= 1.4,
              * and 2 is the tightest integer edge weight that still doesn't undercut it. This
              * matters for PathFindingAlgorithms::DefaultTraversalPolicy::heuristic(), which
              * computes true Euclidean distance for node types with x/y, if diagonal edges
              * carried the same weight as cardinal ones, that heuristic could overestimate the
              * real remaining cost of a diagonal-heavy path and A* would lose its optimality
              * guarantee. Weighting diagonal edges by (at least) their true geometric length
              * keeps the heuristic admissible without having to touch the shared policy code.
              *
              * @tparam Callback callable with signature void(const Edge_t&)
              * @param n const Node_t& source node
              * @param callback callable invoked as callback(const Edge_t&) for each neighbour edge
             */
            template<typename Callback>
            void forEachOutgoingEdge(const Node_t& n, Callback&& callback) const{
                if(n.blocked) return;
                const std::size_t r = n.index / col;
                const std::size_t c = n.index % col;
                // Builds and emits the edge to (nr, nc) with the given weight, only if that
                // coordinate is within bounds and the target node is not blocked.
                auto tryEdge = [&](std::size_t nr, std::size_t nc, int weight){
                     if(nr < row && nc < col){
                         const std::size_t neighbourIndex = nr * col + nc;
                         if(nodes[neighbourIndex].blocked) return;
                         callback(Edge_t(n.index, neighbourIndex, weight));
                     }
                 };
                 // Cardinals: weight 1 (unscaled -- matches the geometric distance exactly).
                 if(r > 0)        tryEdge(r - 1, c,     1);     // North
                 if(r < row - 1)  tryEdge(r + 1, c,     1);     // South
                 if(c > 0)        tryEdge(r, c - 1,     1);     // West
                 if(c < col - 1)  tryEdge(r, c + 1,     1);     // East
                 // Diagonals: weight 2, see @note above.
                 if(diagonal){
                     if(r > 0        && c > 0)        tryEdge(r - 1, c - 1, 2); // NW
                     if(r > 0        && c < col - 1)  tryEdge(r - 1, c + 1, 2); // NE
                     if(r < row - 1  && c > 0)        tryEdge(r + 1, c - 1, 2); // SW
                     if(r < row - 1  && c < col - 1)  tryEdge(r + 1, c + 1, 2); // SE
                 }
             }
             /**
             * @brief Returns a copy of all outgoing (synthesized) edges from node @p n.
             *        Prefer forEachOutgoingEdge when no collection is needed, as that
             *        variant avoids the allocation performed here.
             * @param n const Node_t& source node
             * @return std::vector<Edge_t> all edges whose source is @p n
             */
            std::vector<Edge_t> getOutgoingEdges(const Node_t& n) const{
                std::vector<Edge_t> edges;
                forEachOutgoingEdge(n, [&](const Edge_t& e){ edges.emplace_back(e); });
                return edges;
            }
            /**
             * @brief Removes all outgoing and incoming edges for node @p n.
             *
             * Implemented by setting the stored node's blocked field to true; since
             * forEachOutgoingEdge refuses to yield edges from a blocked node, and refuses
             * to yield edges towards one, this makes @p n behave as fully disconnected,
             * matching EvoAI::Graph::isolate() without needing any separate edge storage
             * to prune. The node itself, and its position in the grid, are otherwise left
             * untouched (no shrinking, no renumbering). There is currently no inverse
             * operation to un-isolate a node.
             * @param n const Node_t& node to isolate
             */
            void isolate(const Node_t& n){
                nodes[n.index].blocked = true;
            }
        private:
            std::size_t row;
            std::size_t col;
            bool diagonal;
            std::vector<Node_t> nodes;
    };
} // namespace EvoAI
#endif // EVOAI_LATTICE_GRAPH_HPP
