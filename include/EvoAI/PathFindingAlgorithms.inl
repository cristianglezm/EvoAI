namespace EvoAI::PathFindingAlgorithms{

	namespace detail{
		/**
		 * @brief Reconstructs a path from @p endNode back to the search root by following
		 *        parent indices stored in @p ctx, then reverses it into start->end order.
		 * @tparam GraphType graph type the search ran over; used only to resolve parent
		 *         indices back into Node_t references via graph[index].
		 * @param graph const GraphType& graph the search ran over (read-only lookup).
		 * @param endNode const Node_t* node the search terminated at; becomes the last
		 *                 element of the returned path.
		 * @param ctx SearchContext& context populated by the search, used to walk
		 *                 came_from links via getParent() until SearchContext::npos is reached.
		 * @return std::vector<const Node_t*> full path including both @p endNode and the
		 *         search root, ordered start->end.
		 */
		template<typename GraphType>
		std::vector<const typename GraphType::Node_t*> reconstructPath(
				const GraphType& graph,
				const typename GraphType::Node_t* endNode,
				SearchContext& ctx) noexcept{
			using Node_t = typename GraphType::Node_t;
			std::vector<const Node_t*> path;
			const Node_t* curr = endNode;
			while(curr != nullptr){
				path.emplace_back(curr);
				auto pIdx = ctx.getParent(curr->index);
				curr = (pIdx != SearchContext::npos) ? &graph[pIdx] : nullptr;
			}
			std::reverse(path.begin(), path.end());
			return path;
		}
	} // namespace detail

	// DFS: stack-based (LIFO); ignores policy.cost/heuristic. Each node is discovered (and
	// thus pushed) at most once, so the isClosed check on pop is defensive, not load-bearing.
	template<typename GraphType, typename Policy>
	std::vector<const typename GraphType::Node_t*> DFS<GraphType, Policy>::operator()(
			const GraphType& graph, const Node_t& start, const Node_t& end,
			SearchContext& ctx, Policy policy) noexcept{
		ctx.reset(graph.size());
		std::vector<const Node_t*> path;
		if(start.index == end.index){
			path.emplace_back(&start);
			return path;
		}
		std::stack<const Node_t*> stack;
		stack.push(&start);
		ctx.discover(start.index, SearchContext::npos, 0.0);
		while(!stack.empty()){
			const auto* v = stack.top();
			stack.pop();
			if(ctx.isClosed(v->index)) continue;
			ctx.close(v->index);
			if(v->index == end.index){
				return detail::reconstructPath(graph, v, ctx);
			}
			graph.forEachOutgoingEdge(*v, [&](const Edge_t& e){
				const auto& nextNode = graph[e.next];
				if(ctx.isClosed(nextNode.index) || !policy.canTraverse(*v, e, nextNode)) return;
				// Discovery gate: each node is discovered (and therefore pushed) exactly once,
				// regardless of how many edges from how many different v's reach it.
				if(!ctx.isDiscovered(nextNode.index)){
					ctx.discover(nextNode.index, v->index, 0.0);
					stack.push(&nextNode);
				}
			});
		}
		return path;
	}

	// BFS: queue-based (FIFO) -> shortest path by hop count; ignores policy.cost/heuristic.
	// Each node is enqueued at most once, so no isClosed guard is needed on pop.
	template<typename GraphType, typename Policy>
	std::vector<const typename GraphType::Node_t*> BFS<GraphType, Policy>::operator()(
			const GraphType& graph, const Node_t& start, const Node_t& end,
			SearchContext& ctx, Policy policy) noexcept{
		ctx.reset(graph.size());
		std::vector<const Node_t*> path;
		if(start.index == end.index){
			path.emplace_back(&start);
			return path;
		}
		std::queue<const Node_t*> queue;
		queue.push(&start);
		ctx.discover(start.index, SearchContext::npos, 0.0);
		while(!queue.empty()){
			const auto* v = queue.front();
			queue.pop();
			ctx.close(v->index);
			if(v->index == end.index){
				return detail::reconstructPath(graph, v, ctx);
			}
			graph.forEachOutgoingEdge(*v, [&](const Edge_t& e){
				const auto& nextNode = graph[e.next];
				if(ctx.isDiscovered(nextNode.index) || !policy.canTraverse(*v, e, nextNode)) return;
				ctx.discover(nextNode.index, v->index, 0.0);
				queue.push(&nextNode);
			});
		}
		return path;
	}

	// GBFS: min-heap on policy.heuristic(node, end) only; ignores policy.cost. The heuristic
	// score is predecessor-independent, so each node is pushed at most once (isDiscovered
	// gate); the isClosed check on pop is defensive, not load-bearing.
	template<typename GraphType, typename Policy>
	std::vector<const typename GraphType::Node_t*> GBFS<GraphType, Policy>::operator()(
			const GraphType& graph, const Node_t& start, const Node_t& end,
			SearchContext& ctx, Policy policy) noexcept{
		ctx.reset(graph.size());
		std::vector<const Node_t*> path;
		if(start.index == end.index){
			path.emplace_back(&start);
			return path;
		}
		// Snapshot heuristic score at push time so the comparator is stable
		// for the lifetime of the priority_queue (mutable ctx captures would break the heap invariant).
		using PQEntry = std::pair<double, const Node_t*>;
		auto comp = [](const PQEntry& a, const PQEntry& b){ return a.first > b.first; };
		std::priority_queue<PQEntry, std::vector<PQEntry>, decltype(comp)> pq(comp);
		ctx.discover(start.index, SearchContext::npos, 0.0);
		pq.push({policy.heuristic(start, end), &start});
		while(!pq.empty()){
			const Node_t* v = pq.top().second;
			pq.pop();
			if(ctx.isClosed(v->index)) continue;
			ctx.close(v->index);
			if(v->index == end.index){
				return detail::reconstructPath(graph, v, ctx);
			}
			graph.forEachOutgoingEdge(*v, [&](const Edge_t& e){
				const auto& nextNode = graph[e.next];
				if(ctx.isDiscovered(nextNode.index) || !policy.canTraverse(*v, e, nextNode)) return;
				ctx.discover(nextNode.index, v->index, 0.0);
				pq.push({policy.heuristic(nextNode, end), &nextNode});
			});
		}
		return path;
	}

	// AStar: min-heap on f = g + h. Unlike GBFS, g is path-dependent, so a node can be
	// legitimately re-relaxed via a cheaper predecessor -> gate on isClosed (not isDiscovered),
	// allowing duplicate pq entries; the isClosed check on pop discards the stale ones.
	template<typename GraphType, typename Policy>
	std::vector<const typename GraphType::Node_t*> AStar<GraphType, Policy>::operator()(
			const GraphType& graph, const Node_t& start, const Node_t& end,
			SearchContext& ctx, Policy policy) noexcept{
		ctx.reset(graph.size());
		std::vector<const Node_t*> path;
		if(start.index == end.index){
			path.emplace_back(&start);
			return path;
		}
		// Snapshot f = g + h at push time so the comparator is stable for the lifetime
		// of the priority_queue. Re-pushed entries (after relaxation) carry the updated
		// f-score; stale entries with higher f-scores are discarded by the isClosed check on pop.
		using PQEntry = std::pair<double, const Node_t*>;
		auto comp = [](const PQEntry& a, const PQEntry& b){ return a.first > b.first; };
		std::priority_queue<PQEntry, std::vector<PQEntry>, decltype(comp)> pq(comp);
		ctx.discover(start.index, SearchContext::npos, 0.0);
		pq.push({policy.heuristic(start, end), &start}); // g=0, f = h(start, end)
		while(!pq.empty()){
			const Node_t* v = pq.top().second;
			pq.pop();
			if(ctx.isClosed(v->index)) continue;
			ctx.close(v->index);
			if(v->index == end.index){
				return detail::reconstructPath(graph, v, ctx);
			}
			graph.forEachOutgoingEdge(*v, [&](const Edge_t& e){
				const auto& nextNode = graph[e.next];
				if(ctx.isClosed(nextNode.index) || !policy.canTraverse(*v, e, nextNode)) return;
				// getCost returns infinity for undiscovered nodes, so this single comparison
				// handles both first discovery and relaxation of an already-open node.
				const double newCost = ctx.getCost(v->index) + policy.cost(*v, e, nextNode);
				if(newCost < ctx.getCost(nextNode.index)){
					ctx.discover(nextNode.index, v->index, newCost);
					pq.push({newCost + policy.heuristic(nextNode, end), &nextNode});
				}
			});
		}
		return path;
	}

} // namespace EvoAI::PathFindingAlgorithms
