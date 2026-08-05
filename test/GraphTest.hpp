#ifndef EVOAI_GRAPH_TEST_HPP
#define EVOAI_GRAPH_TEST_HPP

#include <gtest/gtest.h>
#include <limits>
#include <algorithm>
#include <EvoAI.hpp>
#include <EvoAI/LatticeGraph.hpp>
#include <EvoAI/Utils/TypeUtils.hpp>
#include <EvoAI/PathFindingAlgorithms.hpp>
#include <EvoAI/MazeAlgorithms.hpp>
#include <vector>

namespace EvoAI::Test{
	struct CustomNode{
		CustomNode()
		: index(0)
		, blocked(false)
		, x(0)
		, y(0){}
		CustomNode(JsonBox::Object o)
		: index(static_cast<std::size_t>(std::stoull(o["index"].getString())))
		, blocked(o["blocked"].getBoolean())
		, x(o["x"].getInteger())
		, y(o["y"].getInteger()){}
		JsonBox::Value toJson() const{
			JsonBox::Object o;
			o["index"] = JsonBox::Value(std::to_string(index));
			o["blocked"] = JsonBox::Value(blocked);
			o["x"] = JsonBox::Value(x);
			o["y"] = JsonBox::Value(y);
			return JsonBox::Value(o);
		}
		bool operator==(const CustomNode& rhs) const noexcept{
			return index == rhs.index && blocked == rhs.blocked;
		}
		bool operator!=(const CustomNode& rhs) const noexcept{
			return !(*this == rhs);
		}
		// data
		std::size_t index;
		bool blocked;
		int x;
		int y;
	};
	class CustomNodeGraphTest : public ::testing::Test{
		protected:
			Graph<CustomNode> g{5, 5};
			PathFindingAlgorithms::SearchContext ctx;

			void SetUp() override {
			for(std::size_t i = 0; i < g.size(); ++i){
				g[i].x = static_cast<int>(i % 5);
				g[i].y = static_cast<int>(i / 5);
			}
			for(std::size_t r = 0; r < 5; ++r){
				for(std::size_t c = 0; c < 5; ++c){
					if(r > 0) g.connectToNorth(g[r*5+c], 1);
					if(r < 4) g.connectToSouth(g[r*5+c], 1);
					if(c > 0) g.connectToWest(g[r*5+c], 1);
					if(c < 4) g.connectToEast(g[r*5+c], 1);
				}
			}
		}
	};
	TEST(GraphTest, Constructor){
		Graph<> g;
		EXPECT_EQ(0u, g.size());

		Graph<> g2(10, 10);
		EXPECT_EQ(100u, g2.size());
		EXPECT_EQ(10u,  g2.getRow());
		EXPECT_EQ(10u,  g2.getCol());
	}
	TEST(GraphTest, AddNodeAndEdges){
		Graph<> g;
		g.addNode();
		g.addNode();
		EXPECT_EQ(2u, g.size());

		g.connect(g[0], g[1], 5);
		auto edges = g.getOutgoingEdges(g[0]);
		ASSERT_EQ(1u, edges.size());
		EXPECT_EQ(1u, edges[0].next);
		EXPECT_EQ(5,  edges[0].weight);

		// isConnected checks a single directed edge (n1->n2).
		// areConnected requires edges in BOTH directions.
		EXPECT_TRUE(g.isConnected(g[0], g[1]));
		EXPECT_FALSE(g.isConnected(g[1], g[0]));
	}
	TEST(GraphTest, ConstOperatorIndex){
		Graph<> g(3, 3);
		g[4].blocked = true;
		const Graph<>& cg = g;
		EXPECT_EQ(4u,   cg[4].index);
		EXPECT_TRUE(    cg[4].blocked);
		EXPECT_FALSE(   cg[0].blocked);
	}
	TEST(GraphTest, SetupLattice){
		auto g = Graph<>::createLatticeGraph(10, 10, false);
		EXPECT_EQ(100u, g.size());
		// Node 0 is a corner: East and South only.
		EXPECT_EQ(2u, g.getOutgoingEdges(g[0]).size());
		// Node 11 is interior: N/S/E/W.
		EXPECT_EQ(4u, g.getOutgoingEdges(g[11]).size());
	}
	TEST(GraphTest, ForEachOutgoingEdge){
		Graph<> g;
		g.addNode(); g.addNode(); g.addNode();
		g.connect(g[0], g[1], 10);
		g.connect(g[0], g[2], 20);

		std::vector<std::size_t> nexts;
		g.forEachOutgoingEdge(g[0], [&](const EdgeDefault& e){ nexts.push_back(e.next); });
		ASSERT_EQ(2u, nexts.size());
		EXPECT_TRUE((nexts[0]==1 && nexts[1]==2) || (nexts[0]==2 && nexts[1]==1));

		// Const overload.
		const Graph<>& cg = g;
		std::size_t count = 0;
		cg.forEachOutgoingEdge(cg[0], [&](const EdgeDefault&){ ++count; });
		EXPECT_EQ(2u, count);

		// Node with no edges.
		count = 0;
		g.forEachOutgoingEdge(g[1], [&](const EdgeDefault&){ ++count; });
		EXPECT_EQ(0u, count);
	}
	TEST(GraphTest, IsolateNode){
		auto g = Graph<>::createLatticeGraph(3, 3, false);
		g.isolate(g[4]);

		EXPECT_EQ(0u, g.getOutgoingEdges(g[4]).size());
		for(auto& e : g.getOutgoingEdges(g[1])){
			EXPECT_NE(4u, e.next);
		}
	}
	TEST(SearchContextTest, GenerationCounter){
		namespace PFA = EvoAI::PathFindingAlgorithms;
		PFA::SearchContext ctx;

		ctx.reset(10);
		EXPECT_FALSE(ctx.isDiscovered(5));
		EXPECT_FALSE(ctx.isClosed(5));
		EXPECT_EQ(std::numeric_limits<double>::infinity(), ctx.getCost(5));
		EXPECT_EQ(PFA::SearchContext::npos, ctx.getParent(5));

		ctx.discover(5, PFA::SearchContext::npos, 1.5);
		EXPECT_TRUE(ctx.isDiscovered(5));
		EXPECT_FALSE(ctx.isClosed(5));
		EXPECT_DOUBLE_EQ(1.5, ctx.getCost(5));

		ctx.close(5);
		EXPECT_TRUE(ctx.isClosed(5));

		// reset() is O(1): bumps generation, stale stamps become invalid.
		ctx.reset(10);
		EXPECT_FALSE(ctx.isDiscovered(5));
		EXPECT_FALSE(ctx.isClosed(5));
		EXPECT_EQ(std::numeric_limits<double>::infinity(), ctx.getCost(5));
	}
	TEST(SearchContextTest, GrowsOnDemand){
		namespace PFA = EvoAI::PathFindingAlgorithms;
		PFA::SearchContext ctx;
		ctx.reset(5);
		ctx.discover(4, PFA::SearchContext::npos, 0.0);
		// Grow to a larger graph -- existing stamps invalidated by generation bump.
		ctx.reset(100);
		EXPECT_FALSE(ctx.isDiscovered(4));
		ctx.discover(99, 0, 7.0);
		EXPECT_TRUE(ctx.isDiscovered(99));
		EXPECT_DOUBLE_EQ(7.0, ctx.getCost(99));
	}
	TEST(TypeTraitsTest, IsATraversalPolicy){
		using GraphT = Graph<>;
		using NodeT  = GraphT::Node_t;
		using EdgeT  = GraphT::Edge_t;
		using Policy = PathFindingAlgorithms::DefaultTraversalPolicy<GraphT>;

		EXPECT_TRUE( (meta::is_a_traversal_policy_v<Policy,     NodeT, EdgeT>));
		struct NotAPolicy {};
		EXPECT_FALSE((meta::is_a_traversal_policy_v<NotAPolicy, NodeT, EdgeT>));
	}
	TEST(TypeTraitsTest, IsATraversalPolicy_PartialConformanceFails){
		using GraphT = Graph<>;
		using NodeT  = GraphT::Node_t;
		using EdgeT  = GraphT::Edge_t;
		struct MissingHeuristic{
			bool canTraverse(const NodeT&, const EdgeT&, const NodeT&) const noexcept { return true; }
			double cost(const NodeT&, const EdgeT&, const NodeT&) const noexcept { return 0.0; }
		};
		struct MissingCost{
			bool canTraverse(const NodeT&, const EdgeT&, const NodeT&) const noexcept { return true; }
			double heuristic(const NodeT&, const NodeT&) const noexcept { return 0.0; }
		};
		struct MissingCanTraverse{
			double cost(const NodeT&, const EdgeT&, const NodeT&) const noexcept { return 0.0; }
			double heuristic(const NodeT&, const NodeT&) const noexcept { return 0.0; }
		};
		EXPECT_FALSE((meta::is_a_traversal_policy_v<MissingHeuristic,   NodeT, EdgeT>));
		EXPECT_FALSE((meta::is_a_traversal_policy_v<MissingCost,        NodeT, EdgeT>));
		EXPECT_FALSE((meta::is_a_traversal_policy_v<MissingCanTraverse, NodeT, EdgeT>));
	}
	TEST(TraversalPolicyTest, BlockedAndCost){
		using GraphT = Graph<>;
		using Policy = PathFindingAlgorithms::DefaultTraversalPolicy<GraphT>;

		Graph<> g;
		g.addNode();
		g.addNode();
		g.addNode();
		g[1].blocked = true;
		g.connect(g[0], g[1], 3);
		g.connect(g[0], g[2], 7);

		Policy policy;
		EdgeDefault e01(0,1,3), e02(0,2,7);
		EXPECT_FALSE(policy.canTraverse(g[0], e01, g[1])); // blocked
		EXPECT_TRUE( policy.canTraverse(g[0], e02, g[2])); // passable
		EXPECT_DOUBLE_EQ(3.0, policy.cost(g[0], e01, g[1]));
		EXPECT_DOUBLE_EQ(7.0, policy.cost(g[0], e02, g[2]));
		// NodeDefault has no x/y -> heuristic returns 0 (A* degrades to Dijkstra).
		EXPECT_DOUBLE_EQ(0.0, policy.heuristic(g[0], g[2]));
	}
	TEST(TraversalPolicyTest, EuclideanHeuristicWithXY){
		using GraphT = Graph<CustomNode>;
		using Policy = PathFindingAlgorithms::DefaultTraversalPolicy<GraphT>;

		Graph<CustomNode> g;
		g.addNode(); g.addNode();
		g[0].x = 0; g[0].y = 0;
		g[1].x = 3; g[1].y = 4; // 3-4-5 right triangle

		Policy policy;
		EXPECT_DOUBLE_EQ(5.0, policy.heuristic(g[0], g[1]));
	}
	TEST(PathFindingTest, DFS){
		auto g = Graph<>::createLatticeGraph(5, 5, false);
		PathFindingAlgorithms::SearchContext ctx;
		PathFindingAlgorithms::DFS<Graph<>> dfs;

		auto path = dfs(g, g[0], g[24], ctx);
		ASSERT_FALSE(path.empty());
		EXPECT_EQ(g[0].index,  path.front()->index);
		EXPECT_EQ(g[24].index, path.back()->index);
	}
	// DFS only asserts has_can_traverse_v (see PathFindingAlgorithms.hpp), so a policy that
	// implements nothing else must still compile and work.
	TEST(PathFindingTest, DFS_CanTraverseOnlyPolicy){
		struct CanTraverseOnly{
			bool canTraverse(const NodeDefault&, const EdgeDefault&, const NodeDefault& to) const noexcept{
				return !to.blocked;
			}
		};
		auto g = Graph<>::createLatticeGraph(5, 5, false);
		PathFindingAlgorithms::SearchContext ctx;
		PathFindingAlgorithms::DFS<Graph<>, CanTraverseOnly> dfs;

		auto path = dfs(g, g[0], g[24], ctx, CanTraverseOnly{});
		ASSERT_FALSE(path.empty());
		EXPECT_EQ(g[0].index,  path.front()->index);
		EXPECT_EQ(g[24].index, path.back()->index);
	}
	TEST(PathFindingTest, BFS){
		auto g = Graph<>::createLatticeGraph(10, 10, false);
		PathFindingAlgorithms::SearchContext ctx;
		PathFindingAlgorithms::BFS<Graph<>> bfs;

		auto path = bfs(g, g[0], g[99], ctx);
		ASSERT_FALSE(path.empty());
		EXPECT_EQ(g[0].index,  path.front()->index);
		EXPECT_EQ(g[99].index, path.back()->index);
		// BFS's defining guarantee is shortest path by hop count on an unweighted
		// graph: (0,0) -> (9,9) on a 10x10 lattice is a Manhattan distance of 18
		// edges, i.e. 19 nodes including both endpoints. A non-empty path alone
		// doesn't confirm BFS actually found the *shortest* one.
		EXPECT_EQ(19u, path.size());
	}
    TEST(PathFindingTest, AStar_RespectsWeights){
        Graph<> g;
        g.addNode(); // 0: Start
        g.addNode(); // 1: End
        g.addNode(); // 2: Intermediate waypoint

        // Direct path: very expensive
        g.connect(g[0], g[1], 100.0);

        // Indirect path: very cheap
        g.connect(g[0], g[2], 1.0);
        g.connect(g[2], g[1], 1.0);

        PathFindingAlgorithms::SearchContext ctx;
        PathFindingAlgorithms::AStar<Graph<>> astar;

        auto path = astar(g, g[0], g[1], ctx);

        ASSERT_FALSE(path.empty());
        // A* should ignore the direct (weight 100) edge and take the cheap route (total weight 2)
        ASSERT_EQ(3u, path.size());
        EXPECT_EQ(0u, path[0]->index);
        EXPECT_EQ(2u, path[1]->index);
        EXPECT_EQ(1u, path[2]->index);
    }
	TEST_F(CustomNodeGraphTest, AStar){
		PathFindingAlgorithms::AStar<Graph<CustomNode>> astar;
		auto path = astar(g, g[0], g[24], ctx);
		ASSERT_FALSE(path.empty());
		EXPECT_EQ(g[0].index,  path.front()->index);
		EXPECT_EQ(g[24].index, path.back()->index);
		// With Euclidean heuristic, A* finds the optimal 8-edge path (9 nodes).
		EXPECT_EQ(9u, path.size());
	}
	TEST_F(CustomNodeGraphTest, GBFS){
		PathFindingAlgorithms::GBFS<Graph<CustomNode>> gbfs;
		auto path = gbfs(g, g[0], g[24], ctx);
		ASSERT_FALSE(path.empty());
		EXPECT_EQ(g[0].index,  path.front()->index);
		EXPECT_EQ(g[24].index, path.back()->index);
	}
	TEST_F(CustomNodeGraphTest, SearchContextReuse){
		// Shared ctx must isolate searches via generation counter.
		PathFindingAlgorithms::BFS<Graph<CustomNode>> bfs;
		auto path1 = bfs(g, g[0],  g[24], ctx);
		auto path2 = bfs(g, g[24], g[0],  ctx);
		EXPECT_FALSE(path1.empty());
		EXPECT_FALSE(path2.empty());
		EXPECT_EQ(g[24].index, path1.back()->index);
		EXPECT_EQ(g[0].index,  path2.back()->index);
	}
	TEST_F(CustomNodeGraphTest, ConstNodePointerReturn){
		PathFindingAlgorithms::AStar<Graph<CustomNode>> astar;
		auto path = astar(g, g[0], g[24], ctx);
		static_assert(
			std::is_same_v<decltype(path), std::vector<const CustomNode*>>,
			"Path must be vector<const Node_t*>");
		EXPECT_FALSE(path.empty());
	}
	TEST(LatticeGraphTest, Basics){
		LatticeGraph<> g(10, 10);
		EXPECT_EQ(100u, g.size());
		EXPECT_EQ(10u,  g.getRow());
		EXPECT_EQ(10u,  g.getCol());
		EXPECT_EQ(5u,   g[5].index);

		// Corner node: South and East only.
		EXPECT_EQ(2u, g.getOutgoingEdges(g[0]).size());

		// Center node of a 3x3 diagonal graph has 8 neighbours.
		LatticeGraph<> gDiag(3, 3, true);
		EXPECT_EQ(8u, gDiag.getOutgoingEdges(gDiag[4]).size());
	}
	TEST(LatticeGraphTest, ForEachOutgoingEdge){
		LatticeGraph<> g(3, 3, false);

		std::size_t c0 = 0;
		g.forEachOutgoingEdge(g[0], [&](const EdgeDefault&){ ++c0; });
		EXPECT_EQ(2u, c0); // corner

		std::vector<EdgeDefault> edges;
		g.forEachOutgoingEdge(g[4], [&](const EdgeDefault& e){ edges.push_back(e); });
		ASSERT_EQ(4u, edges.size());
		for(const auto& e : edges) EXPECT_EQ(4u, e.prev);

		// Const overload.
		const LatticeGraph<>& cg = g;
		std::size_t cc = 0;
		cg.forEachOutgoingEdge(cg[4], [&](const EdgeDefault&){ ++cc; });
		EXPECT_EQ(4u, cc);
	}
	// LatticeGraph has no edge container to prune, so isolate() is backed by
	// Node_t::blocked instead: forEachOutgoingEdge must refuse edges both from
	// and towards a blocked node.
	TEST(LatticeGraphTest, IsolateNode){
		LatticeGraph<> g(3, 3, false);
		g.isolate(g[4]);

		EXPECT_TRUE(g[4].blocked);
		EXPECT_EQ(0u, g.getOutgoingEdges(g[4]).size());
		for(auto& e : g.getOutgoingEdges(g[1])) EXPECT_NE(4u, e.next);
	}
	// Diagonal weight (2) must stay >= the true geometric diagonal distance (sqrt(2)) so
	// PathFindingAlgorithms::DefaultTraversalPolicy's Euclidean heuristic remains admissible
	// see the @note on forEachOutgoingEdge. Cardinal weight stays unscaled at 1.
	TEST(LatticeGraphTest, DiagonalEdgeWeights){
		LatticeGraph<> g(3, 3, true);
		std::vector<EdgeDefault> edges;
		g.forEachOutgoingEdge(g[4], [&](const EdgeDefault& e){ edges.push_back(e); }); // center: 8 neighbours
		ASSERT_EQ(8u, edges.size());
		std::size_t cardinalCount = 0, diagonalCount = 0;
		for(const auto& e : edges){
			if(e.weight == 1)      ++cardinalCount;
			else if(e.weight == 2) ++diagonalCount;
			else                   FAIL() << "unexpected edge weight " << e.weight;
		}
		EXPECT_EQ(4u, cardinalCount);
		EXPECT_EQ(4u, diagonalCount);
	}
	// Regression guard for the same issue: if diagonal edges ever went back to weight 1
	// while x/y-based heuristics compute real Euclidean distance, the heuristic would
	// overestimate diagonal-heavy paths and A* could return a longer, cardinal-only route
	// instead of the true shortest (all-diagonal) one. Confirm it still finds the optimal path.
	TEST(LatticeGraphTest, DiagonalAStarFindsOptimalPath){
		LatticeGraph<CustomNode> g(6, 6, true);
		for(std::size_t i = 0; i < g.size(); ++i){
			g[i].x = static_cast<int>(i % 6);
			g[i].y = static_cast<int>(i / 6);
		}
		PathFindingAlgorithms::SearchContext ctx;
		PathFindingAlgorithms::AStar<LatticeGraph<CustomNode>> astar;
		auto path = astar(g, g[0], g[35], ctx); // (0,0) -> (5,5): a pure diagonal line exists

		ASSERT_FALSE(path.empty());
		// Optimal route is 5 pure-diagonal hops (cost 5*2=10, beats any cardinal detour).
		EXPECT_EQ(6u, path.size());
		int totalCost = 0;
		for(std::size_t i = 1; i < path.size(); ++i){
			int dx = path[i]->x - path[i-1]->x; if(dx < 0) dx = -dx;
			int dy = path[i]->y - path[i-1]->y; if(dy < 0) dy = -dy;
			totalCost += (dx == 1 && dy == 1) ? 2 : 1;
		}
		EXPECT_EQ(10, totalCost);
	}
	TEST(LatticeGraphTest, PathFinding){
		LatticeGraph<> g(10, 10);
		PathFindingAlgorithms::SearchContext ctx;
		PathFindingAlgorithms::BFS<LatticeGraph<>> bfs;

		auto path = bfs(g, g[0], g[99], ctx);
		ASSERT_FALSE(path.empty());
		EXPECT_EQ(g[0].index,  path.front()->index);
		EXPECT_EQ(g[99].index, path.back()->index);
		// Same shortest-path guarantee as the plain Graph<> BFS test above.
		EXPECT_EQ(19u, path.size());
	}
	TEST(MazeAlgorithmsTest, GrowingTree){
		EvoAI::randomGen().setSeed(1337); // deterministic for tests

		auto g = Graph<>::createLatticeGraph(10, 10, false);
		MazeAlgorithms::GrowingTree<Graph<>> growingTree;
		growingTree(g, g[0]);

		bool hasBlocked = false;
		for (std::size_t i = 0; i < g.size(); ++i) {
			if (g[i].blocked) { hasBlocked = true; break; }
		}
		EXPECT_TRUE(hasBlocked);
	}
	// The origin is the one cell GrowingTree starts carving from and never
	// revisits as a candidate; it should never end up isolated as a wall.
	TEST(MazeAlgorithmsTest, GrowingTree_OriginNeverBlocked){
		EvoAI::randomGen().setSeed(7);
		auto g = Graph<>::createLatticeGraph(10, 10, false);
		MazeAlgorithms::GrowingTree<Graph<>> growingTree;
		growingTree(g, g[0]);

		EXPECT_FALSE(g[0].blocked);
	}
	TEST(MazeAlgorithmsTest, GrowingTree_ProducesNonTrivialMaze){
		EvoAI::randomGen().setSeed(7);
		auto g = Graph<>::createLatticeGraph(10, 10, false);
		MazeAlgorithms::GrowingTree<Graph<>> growingTree;
		growingTree(g, g[0]);

		std::size_t unblockedCount = 0;
		for (std::size_t i = 0; i < g.size(); ++i) {
			if (!g[i].blocked) ++unblockedCount;
		}
		// Sanity: maze should have a meaningful number of open cells
		EXPECT_GT(unblockedCount, g.size() / 4u);
	}
	TEST(MazeAlgorithmsTest, GrowingTree_OriginHasLargeConnectedComponent){
		EvoAI::randomGen().setSeed(7);
		auto g = Graph<>::createLatticeGraph(10, 10, false);
		MazeAlgorithms::GrowingTree<Graph<>> growingTree;
		growingTree(g, g[0]);

		PathFindingAlgorithms::BFS<Graph<>> bfs;
		PathFindingAlgorithms::SearchContext ctx;
		
		std::size_t reachableCount = 0;
		for(std::size_t i = 0; i < g.size(); ++i){
			if(g[i].blocked) continue;
			auto path = bfs(g, g[0], g[i], ctx);
			if(!path.empty()) ++reachableCount;
		}
		
		// Origin's connected region should be a meaningful chunk of the maze
		EXPECT_GT(reachableCount, g.size() / 4u);
	}
	TEST(MazeAlgorithmsTest, GrowingTree_DeterministicForSameSeed){
		MazeAlgorithms::GrowingTree<Graph<>> growingTree;

		EvoAI::randomGen().setSeed(2024);
		auto g1 = Graph<>::createLatticeGraph(8, 8, false);
		growingTree(g1, g1[0], 0.3f, 0.4f);

		EvoAI::randomGen().setSeed(2024);
		auto g2 = Graph<>::createLatticeGraph(8, 8, false);
		growingTree(g2, g2[0], 0.3f, 0.4f);

		ASSERT_EQ(g1.size(), g2.size());
		auto byNext = [](const EdgeDefault& a, const EdgeDefault& b){ return a.next < b.next; };
		for (std::size_t i = 0; i < g1.size(); ++i) {
			EXPECT_EQ(g1[i].blocked, g2[i].blocked) << "blocked mismatch at node " << i;

			auto e1 = g1.getOutgoingEdges(g1[i]);
			auto e2 = g2.getOutgoingEdges(g2[i]);
			std::sort(e1.begin(), e1.end(), byNext);
			std::sort(e2.begin(), e2.end(), byNext);
			ASSERT_EQ(e1.size(), e2.size()) << "edge count mismatch at node " << i;
			for (std::size_t k = 0; k < e1.size(); ++k) {
				EXPECT_EQ(e1[k].next, e2[k].next) << "node " << i << " edge " << k;
			}
		}
	}
	TEST(MazeAlgorithmsTest, GrowingTree_ClampsOutOfRangeProbabilities){
		EvoAI::randomGen().setSeed(55);
		auto g = Graph<>::createLatticeGraph(8, 8, false);
		MazeAlgorithms::GrowingTree<Graph<>> growingTree;
		growingTree(g, g[0], -10.0f, 99.0f);

		EXPECT_FALSE(g[0].blocked);

		std::size_t unblockedCount = 0;
		for (std::size_t i = 0; i < g.size(); ++i) {
			if (!g[i].blocked) ++unblockedCount;
		}
		// Sanity: extreme clamped inputs shouldn't result in a fully blocked maze
		EXPECT_GT(unblockedCount, g.size() / 4u);
	}
	TEST(MazeAlgorithmsTest, GrowingTree_OnLatticeGraph){
		EvoAI::randomGen().setSeed(123);
		LatticeGraph<> g(8, 8, false);
		MazeAlgorithms::GrowingTree<LatticeGraph<>> growingTree;
		growingTree(g, g[0]);

		EXPECT_FALSE(g[0].blocked);

		std::size_t unblockedCount = 0;
		for (std::size_t i = 0; i < g.size(); ++i) {
			if (!g[i].blocked) ++unblockedCount;
		}
		// Sanity: ensure it actually carved a meaningful maze on LatticeGraph
		EXPECT_GT(unblockedCount, g.size() / 4u);
	}
	TEST_F(CustomNodeGraphTest, AStar_NoPathReturnsEmpty){
		// Sever the only connection between the left half and right half of the 5x5 grid
		g[2].blocked = true;
		g[7].blocked = true;
		g[12].blocked = true;
		g[17].blocked = true;
		g[22].blocked = true;

		PathFindingAlgorithms::AStar<Graph<CustomNode>> astar;
		// Node 0 is top-left, Node 4 is top-right. They are now completely isolated.
		auto path = astar(g, g[0], g[4], ctx);
		// The user contract: no crash, no infinite loop, just an empty path.
		EXPECT_TRUE(path.empty());
	}
} // namespace EvoAI::Test

#endif // EVOAI_GRAPH_TEST_HPP
