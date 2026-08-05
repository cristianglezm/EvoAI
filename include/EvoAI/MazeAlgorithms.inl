namespace EvoAI::MazeAlgorithms{
	template<typename GraphType>
	void GrowingTree<GraphType>::operator()(GraphType& graph, Node_t& origin, float randomness, float binomial) noexcept{
		randomness = std::clamp(randomness, 0.0f, 1.0f);
		binomial = std::clamp(binomial, 0.0f, 1.0f);
		std::vector<uint8_t> visited(graph.size(), 0u);
		std::vector<Node_t*> cells;
		cells.reserve(graph.size() / 4);
		cells.emplace_back(&origin);
		visited[origin.index] = 1u;
		auto& rg = randomGen();
		std::bernoulli_distribution bdRandomness(static_cast<double>(randomness));
		std::bernoulli_distribution bdBinomial(static_cast<double>(binomial));
		std::vector<Node_t*> unvisited;
		while(!cells.empty()){
			// Pick current cell: uniformly random (Prim's-like) or most-recently-added (DFS-like).
			Node_t* cell = bdRandomness(rg.getEngine())
				? cells[std::uniform_int_distribution<std::size_t>(0, cells.size() - 1)(rg.getEngine())]
				: cells.back();
			unvisited.clear();
			graph.forEachOutgoingEdge(*cell, [&](const Edge_t& e){
				Node_t& nb = graph[e.next];
				if(!visited[nb.index]){
					unvisited.emplace_back(&nb);
				}
			});
			if(!unvisited.empty()){
				const std::size_t nvSize = unvisited.size();

				// Pick a neighbour to carve into. Its existing edge to `cell` is left as-is
				// (this is a subtractive carver: passages are whatever edges survive, not
				// edges added here), and it joins the frontier so carving can continue from it.
				const std::size_t pickIndex = bdBinomial(rg.getEngine())
					? std::binomial_distribution<std::size_t>(nvSize - 1, static_cast<double>(binomial))(rg.getEngine())
					: std::uniform_int_distribution<std::size_t>(0, nvSize - 1)(rg.getEngine());

				Node_t& carved = *unvisited[pickIndex];
				visited[carved.index] = 1u;
				cells.emplace_back(&carved);

				// From the remaining unvisited neighbours (excluding the one just carved),
				// randomly pick at most one to turn into a wall: mark it visited so it's
				// never reconsidered, flag it blocked, and isolate() it to strip all of its
				// edges from the graph.
				unvisited.erase(unvisited.begin() + pickIndex);
				if(!unvisited.empty()){
					const std::size_t pick2 = std::uniform_int_distribution<std::size_t>(0, unvisited.size() - 1)(rg.getEngine());
					Node_t& blocked = *unvisited[pick2];
					visited[blocked.index] = 1u;
					blocked.blocked = true;
					graph.isolate(blocked);
				}
			}else{
				// No unvisited neighbours left: this cell is exhausted, backtrack by
				// dropping it from the frontier (pointer identity, not value equality).
				cells.erase(std::remove(cells.begin(), cells.end(), cell), cells.end());
			}
		}
	}
} // namespace EvoAI::MazeAlgorithms
