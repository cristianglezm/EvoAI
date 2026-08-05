#ifndef EVOAI_GRAPH_HPP
#define EVOAI_GRAPH_HPP

#include <vector>
#include <string>
#include <limits>
#include <cstddef>

#include <JsonBox.h>

#include <EvoAI/Utils/TypeUtils.hpp>
#include <EvoAI/Utils/RandomUtils.hpp>

namespace EvoAI{
	/**
	 * @brief Default Node type satisfying meta::is_a_graph_node_v.
	 *
	 * Can be used directly or as a reference for custom node types. Custom nodes
	 * must provide: a JsonBox::Object constructor, a toJson() method returning
	 * JsonBox::Value, and an accessible index field.
	 */
	struct NodeDefault{
		/**
		 * @brief Default constructor. Initializes index to 0 and blocked to false.
		 */
		NodeDefault();
		/**
		 * @brief Construct from a serialized JsonBox::Object.
		 * @param o JsonBox::Object serialized node data
		 */
		NodeDefault(JsonBox::Object o);
		/**
		 * @brief Serialize this node to JsonBox::Value.
		 * @return JsonBox::Value serialized representation
		 */
		JsonBox::Value toJson() const;
		bool operator==(const NodeDefault& rhs) const noexcept;
		bool operator!=(const NodeDefault& rhs) const noexcept;
		// data
		std::size_t index;
		bool blocked;
	};
	/**
	 * @brief Default Edge type satisfying meta::is_a_graph_edge_v.
	 *
	 * Can be used directly or as a reference for custom edge types. Custom edges
	 * must provide: a JsonBox::Object constructor, a toJson() method returning
	 * JsonBox::Value, and the fields prev, next, and weight.
	 *
	 * @details The EdgeDefault stores the flat indices of the source and
	 *          destination nodes in **prev** and **next**, and an integer **weight**.
	 *
	 * @param p flat index of the source node
	 * @param n flat index of the destination node
	 * @param w weight assigned to this edge
	 */
	struct EdgeDefault{
		/** 
		 * @brief Default constructor.
		 */
		EdgeDefault();
		/**
		 * @brief Construct an edge connecting two nodes with a given weight.
		 * @param p const std::size_t& flat index of the source node
		 * @param n const std::size_t& flat index of the destination node
		 * @param w const int& weight assigned to this edge
		 */
		EdgeDefault(const std::size_t& p, const std::size_t& n, const int& w);
		/**
		 * @brief Construct from a serialized JsonBox::Object.
		 * @param o JsonBox::Object serialized edge data
		 */
		EdgeDefault(JsonBox::Object o);
		/**
		 * @brief Serialize this edge to JsonBox::Value.
		 * @return JsonBox::Value serialized representation
		 */
		JsonBox::Value toJson() const;
		bool operator==(const EdgeDefault& rhs) const noexcept;
		bool operator!=(const EdgeDefault& rhs) const noexcept;
		bool operator<=(const EdgeDefault& rhs) const noexcept;
		// data
		int weight;
		std::size_t prev;
		std::size_t next;
	};
	/**
	 * @brief Templated graph using a single contiguous forward‑star edge store.
     * @author Cristian Gonzalez <cristian.glez.m@gmail.com>
	 *
	 * @tparam NodeType  Node type; must satisfy meta::is_a_graph_node_v (toJson(), JsonBox::Object ctor, integral index). Defaults to NodeDefault.
	 * @tparam EdgeType  Edge type; must satisfy meta::is_a_graph_edge_v (toJson(), JsonBox::Object ctor, integral prev/next/weight). Defaults to EdgeDefault.
	 *
	 * @details The graph stores edges in a forward‑star layout and exposes node/edge accessors
	 *          expected by the algorithms in EvoAI.
	 *
	 * @warning Write operations from multiple threads must be guarded.
	 */
    template<typename NodeType = NodeDefault, typename EdgeType = EdgeDefault>
    class Graph{
        static_assert(meta::is_a_graph_node_v<NodeType>, "NodeType must satisfy meta::is_a_graph_node_v (toJson, ctor(JsonBox::Object), index).");
        static_assert(meta::is_a_graph_edge_v<EdgeType>, "EdgeType must satisfy meta::is_a_graph_edge_v (toJson, ctor(JsonBox::Object), prev, next, weight).");
        public:
            using Node_t = NodeType;
            using Edge_t = EdgeType;
        public:
            /**
             * @brief Creates an empty Graph with no nodes and no edges.
             */
            explicit Graph();
            /**
             * @brief Creates a grid-shaped Graph with row * col nodes and no edges.
             * @param row const std::size_t& number of rows in the grid layout
             * @param col const std::size_t& number of columns in the grid layout
             */
            explicit Graph(const std::size_t& row, const std::size_t& col);
            /**
             * @brief Constructs a Graph from a serialized JsonBox::Object.
             * @param o JsonBox::Object previously produced by toJson()
             */
            Graph(JsonBox::Object o);
            /**
             * @brief Serializes the graph (nodes, edges, row, col) to a JsonBox::Value.
             * @return JsonBox::Value serialized representation of this graph
             */
            JsonBox::Value toJson() const;
            /**
             * @brief Writes the serialized graph to the specified file.
             * @param filename const std::string& path of the output file
             */
            void writeToFile(const std::string& filename) const;
            /**
             * @brief Appends a node to the graph. Does not update row or col.
             * @param n Node_t node to add; defaults to a default-constructed Node_t
             */
            void addNode(Node_t n = Node_t());
            /**
             * @brief Returns a reference to the node at grid position (x, y).
             * @param x const std::size_t& row coordinate
             * @param y const std::size_t& column coordinate
             * @return Node_t& reference to the node at (x, y)
             */
            Node_t& getNode(const std::size_t& x, const std::size_t& y);
            /**
             * @brief Returns pointers to all nodes that @p n has an outgoing edge to.
             * @param n const Node_t& source node
             * @return std::vector<Node_t*> neighbours reachable from @p n
             */
            std::vector<Node_t*> getNeighbours(const Node_t& n);
            /**
             * @brief Converts a node's flat index to a (row, col) grid coordinate pair.
             * @param n const Node_t& node whose index is to be decomposed
             * @return std::pair<std::size_t, std::size_t> (row, col) coordinates of @p n
             */
            std::pair<std::size_t,std::size_t> getIndex2d(const Node_t& n);
            /**
             * @brief Removes all edges from and to @p n, effectively isolating it.
             *        Does not shrink the node array or modify row / col.
             * @param n const Node_t& node to remove
             */
            void removeNode(const Node_t& n);
            /**
             * @brief Returns the total number of nodes in the graph.
             * @return std::size_t number of nodes
             */
            std::size_t size() const noexcept;
            /**
             * @brief Returns a reference to the node at the given flat index.
             * @param index const std::size_t& flat node index
             * @return Node_t& reference to the node
             */
            Node_t& operator[](const std::size_t& index);
            /**
             * @brief Returns a const reference to the node at the given flat index.
             * @param index const std::size_t& flat node index
             * @return const Node_t& const reference to the node
             */
            const Node_t& operator[](const std::size_t& index) const;
            /**
             * @brief Returns a copy of all outgoing edges from node @p n.
             *        Prefer forEachOutgoingEdge when no collection is needed,
             *        as that variant avoids the allocation.
             * @param n const Node_t& source node
             * @return const std::vector<Edge_t> all edges whose source is @p n
             */
            const std::vector<Edge_t> getOutgoingEdges(const Node_t& n) const;
            /**
             * @brief Iterates over all outgoing edges of @p n without allocating.
             *
             * Walks the forward-star linked list for @p n and invokes @p callback
             * once per edge. Prefer this over getOutgoingEdges in performance-sensitive
             * paths (e.g. inside search algorithms).
             *
             * @tparam Callback callable with signature void(const Edge_t&)
             * @param n const Node_t& source node
             * @param callback callable invoked for each outgoing edge of @p n
             */
            template<typename Callback>
            void forEachOutgoingEdge(const Node_t& n, Callback&& callback) const;
            /**
             * @brief Adds a single directed edge from e.prev to e.next.
             * @param e const Edge_t& edge to add
             */
            void addDirectedEdge(const Edge_t& e);
            /**
             * @brief Adds two directed edges: one from e.prev to e.next
             *        and one from e.next to e.prev.
             * @param e const Edge_t& edge whose endpoints are connected bidirectionally
             */
            void addUndirectedEdge(const Edge_t& e);
            /**
             * @brief Removes the directed edge from e.prev to e.next, if it exists.
             * @param e const Edge_t& edge to remove
             */
            void removeDirectedEdge(const Edge_t& e);
            /**
             * @brief Removes both directed edges between e.prev and e.next, if they exist.
             * @param e const Edge_t& edge whose endpoints are to be disconnected bidirectionally
             */
            void removeUndirectedEdge(const Edge_t& e);
            /**
             * @brief Removes all outgoing edges from node @p n.
             * @param n const Node_t& node whose outgoing edges are removed
             */
            void removeAllEdges(const Node_t& n);
            /**
             * @brief Removes all outgoing and all incoming edges for node @p n.
             * @param n const Node_t& node to fully disconnect from the graph
             */
            void isolate(const Node_t& n);
            /**
             * @brief Checks whether @p n1 and @p n2 are connected to each other (bidirectional).
             * @param n1 const Node_t& first node
             * @param n2 const Node_t& second node
             * @return bool true if @p n1 is connected to @p n2 and vice versa
             */
            bool areConnected(const Node_t& n1, const Node_t& n2) const;
            /**
             * @brief Checks whether there is a directed edge from @p n1 to @p n2.
             * @param n1 const Node_t& source node
             * @param n2 const Node_t& destination node
             * @return bool true if @p n1 has an outgoing edge reaching @p n2
             */
            bool isConnected(const Node_t& n1, const Node_t& n2) const;
            /**
             * @brief Sets the number of rows in the grid layout.
             * @param r const std::size_t& new row count
             */
            void setRow(const std::size_t& r);
            /**
             * @brief Returns the number of rows in the grid layout.
             * @return const std::size_t& current row count
             */
            const std::size_t& getRow() const;
            /**
             * @brief Sets the number of columns in the grid layout.
             * @param c const std::size_t& new column count
             */
            void setCol(const std::size_t& c);
            /**
             * @brief Returns the number of columns in the grid layout.
             * @return const std::size_t& current column count
             */
            const std::size_t& getCol() const;
            /**
             * @brief Connects @p n1 to @p n2 with a directed edge if not already connected.
             * @param n1 Node_t& source node
             * @param n2 Node_t& destination node
             * @param weight int weight assigned to the edge (default 0)
             */
            void connect(Node_t& n1, Node_t& n2, int weight = 0) noexcept;
            /**
             * @brief Connects @p n1 and @p n2 to each other if not already connected bidirectionally.
             * @param n1 Node_t& first node
             * @param n2 Node_t& second node
             * @param weight int weight assigned to both edges (default 0)
             */
            void connectToEachOther(Node_t& n1, Node_t& n2, int weight = 0) noexcept;
            /**
             * @brief Removes the directed edge from @p n1 to @p n2 if it exists.
             * @param n1 Node_t& source node
             * @param n2 Node_t& destination node
             */
            void disconnect(Node_t& n1, Node_t& n2) noexcept;
            /**
             * @brief Removes directed edges between @p n1 and @p n2 in both directions if they exist.
             * @param n1 Node_t& first node
             * @param n2 Node_t& second node
             */
            void disconnectFromEachOther(Node_t& n1, Node_t& n2) noexcept;
            /**
             * @brief Connects @p n to its northern neighbour if one exists.
             *        Relies on row and col being correctly set.
             * @param n Node_t& source node
             * @param weight int weight assigned to the edge (default 0)
             */
            void connectToNorth(Node_t& n, int weight = 0) noexcept;
            /**
             * @brief Connects @p n to its north-east neighbour if one exists.
             *        Relies on row and col being correctly set.
             * @param n Node_t& source node
             * @param weight int weight assigned to the edge (default 0)
             */
            void connectToNorthEast(Node_t& n, int weight = 0) noexcept;
            /**
             * @brief Connects @p n to its north-west neighbour if one exists.
             *        Relies on row and col being correctly set.
             * @param n Node_t& source node
             * @param weight int weight assigned to the edge (default 0)
             */
            void connectToNorthWest(Node_t& n, int weight = 0) noexcept;
            /**
             * @brief Connects @p n to its eastern neighbour if one exists.
             *        Relies on row and col being correctly set.
             * @param n Node_t& source node
             * @param weight int weight assigned to the edge (default 0)
             */
            void connectToEast(Node_t& n, int weight = 0) noexcept;
            /**
             * @brief Connects @p n to its western neighbour if one exists.
             *        Relies on row and col being correctly set.
             * @param n Node_t& source node
             * @param weight int weight assigned to the edge (default 0)
             */
            void connectToWest(Node_t& n, int weight = 0) noexcept;
            /**
             * @brief Connects @p n to its southern neighbour if one exists.
             *        Relies on row and col being correctly set.
             * @param n Node_t& source node
             * @param weight int weight assigned to the edge (default 0)
             */
            void connectToSouth(Node_t& n, int weight = 0) noexcept;
            /**
             * @brief Connects @p n to its south-east neighbour if one exists.
             *        Relies on row and col being correctly set.
             * @param n Node_t& source node
             * @param weight int weight assigned to the edge (default 0)
             */
            void connectToSouthEast(Node_t& n, int weight = 0) noexcept;
            /**
             * @brief Connects @p n to its south-west neighbour if one exists.
             *        Relies on row and col being correctly set.
             * @param n Node_t& source node
             * @param weight int weight assigned to the edge (default 0)
             */
            void connectToSouthWest(Node_t& n, int weight = 0) noexcept;
            /**
             * @brief Connects @p n to all cardinal neighbours, and optionally to diagonal ones.
             *        Relies on row and col being correctly set.
             * @param n Node_t& source node
             * @param weight int weight assigned to all created edges (default 0)
             * @param diagonal bool if true, also connects to the four diagonal neighbours (default false)
             */
            void connectToNeighbours(Node_t& n, int weight = 0, bool diagonal = false) noexcept;
            /**
             * @brief Returns a pointer to the node directly north of @p n, or nullptr if none.
             *        Relies on row and col being correctly set.
             * @param n Node_t& reference node
             * @return Node_t* pointer to the northern neighbour, or nullptr
             */
            Node_t* getNorth(Node_t& n) noexcept;
            /**
             * @brief Returns a pointer to the node north-east of @p n, or nullptr if none.
             *        Relies on row and col being correctly set.
             * @param n Node_t& reference node
             * @return Node_t* pointer to the north-east neighbour, or nullptr
             */
            Node_t* getNorthEast(Node_t& n) noexcept;
            /**
             * @brief Returns a pointer to the node north-west of @p n, or nullptr if none.
             *        Relies on row and col being correctly set.
             * @param n Node_t& reference node
             * @return Node_t* pointer to the north-west neighbour, or nullptr
             */
            Node_t* getNorthWest(Node_t& n) noexcept;
            /**
             * @brief Returns a pointer to the node directly east of @p n, or nullptr if none.
             *        Relies on row and col being correctly set.
             * @param n Node_t& reference node
             * @return Node_t* pointer to the eastern neighbour, or nullptr
             */
            Node_t* getEast(Node_t& n) noexcept;
            /**
             * @brief Returns a pointer to the node directly west of @p n, or nullptr if none.
             *        Relies on row and col being correctly set.
             * @param n Node_t& reference node
             * @return Node_t* pointer to the western neighbour, or nullptr
             */
            Node_t* getWest(Node_t& n) noexcept;
            /**
             * @brief Returns a pointer to the node directly south of @p n, or nullptr if none.
             *        Relies on row and col being correctly set.
             * @param n Node_t& reference node
             * @return Node_t* pointer to the southern neighbour, or nullptr
             */
            Node_t* getSouth(Node_t& n) noexcept;
            /**
             * @brief Returns a pointer to the node south-east of @p n, or nullptr if none.
             *        Relies on row and col being correctly set.
             * @param n Node_t& reference node
             * @return Node_t* pointer to the south-east neighbour, or nullptr
             */
            Node_t* getSouthEast(Node_t& n) noexcept;
            /**
             * @brief Returns a pointer to the node south-west of @p n, or nullptr if none.
             *        Relies on row and col being correctly set.
             * @param n Node_t& reference node
             * @return Node_t* pointer to the south-west neighbour, or nullptr
             */
            Node_t* getSouthWest(Node_t& n) noexcept;
            /**
             * @brief Returns pointers to all grid neighbours of @p n (cardinal and optionally diagonal).
             *        Relies on row and col being correctly set.
             * @param n Node_t& reference node
             * @param diagonal bool if true, includes the four diagonal neighbours (default false)
             * @return std::vector<Node_t*> all existing neighbours at the requested cardinal points
             */
            std::vector<Node_t*> getAllNeighbours(Node_t& n, bool diagonal = false) noexcept;
            /**
             * @brief Factory method that creates a fully connected lattice graph.
             *        Each node is connected to its 4 cardinal neighbours,
             *        or to all 8 neighbours when diagonal is true.
             * @param row const std::size_t& number of rows
             * @param col const std::size_t& number of columns
             * @param diagonal bool if true, also creates diagonal connections (default false)
             * @return Graph a row * col lattice graph with edges already inserted
             */
            static Graph createLatticeGraph(const std::size_t& row, const std::size_t& col, bool diagonal = false);
            bool operator==(const Graph& rhs) const;
            bool operator!=(const Graph& rhs) const;
            ~Graph() = default;
        private:
            /**
             * @brief Internal edge record used by the forward-star store.
             *
             * Each InternalEdge pairs the user-visible edge payload with a singly-linked
             * list pointer (nextIndex) that chains all outgoing edges of the same source
             * node together. npos signals the end of the list.
             */
            struct InternalEdge{
                Edge_t payload;
                std::size_t nextIndex;
            };
			static constexpr std::size_t npos = std::numeric_limits<std::size_t>::max();
        private:
            std::vector<Node_t> nodes;
            std::vector<InternalEdge> edges;
            std::vector<std::size_t> head;
            std::size_t row;
            std::size_t col;
    };
} // namespace EvoAI

#include "Graph.inl"

#endif // EVOAI_GRAPH_HPP
