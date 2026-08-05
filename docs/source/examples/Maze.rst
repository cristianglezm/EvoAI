.. default-domain:: cpp
.. highlight:: cpp

Maze
=====

In this example letters travel from the center of a randomly generated maze to a row of
targets at the bottom, spelling out a piece of text supplied on the command line. Each
letter is routed independently with a randomly chosen search algorithm — :class:`DFS<EvoAI::PathFindingAlgorithms::DFS>`,
:class:`BFS<EvoAI::PathFindingAlgorithms::BFS>`, :class:`GBFS<EvoAI::PathFindingAlgorithms::GBFS>`
or :class:`AStar<EvoAI::PathFindingAlgorithms::AStar>` — and all letters search and animate
concurrently on a small thread pool, so the demo doubles as a side-by-side race between the
four algorithms.

::

    Maze "hello, world!" [--seed N] [--randomness F] [--binomial F] [-w N] [-h N]

Building the maze
------------------

A maze is exactly the case :class:`LatticeGraph<EvoAI::LatticeGraph>` exists for: a regular
grid whose edges are always just its cardinal neighbours. Rather than the general-purpose
:class:`Graph<EvoAI::Graph>` (a forward-star edge store, built for arbitrary/explicit
topology), the maze uses ``LatticeGraph``, which derives edges from index math and each
node's ``blocked`` flag on every call instead of storing them — no edges to allocate when
the grid is built, and no forward-star fixup cost when :func:`isolate<EvoAI::LatticeGraph::isolate>`
walls a cell off during carving.

The node type is a small custom struct, ``MazeNode``, rather than the library's
:struct:`NodeDefault<EvoAI::NodeDefault>`:

.. code-block:: cpp

    struct MazeNode {
        MazeNode() = default;
        MazeNode(JsonBox::Object o) : /* ... */ {}
        JsonBox::Value toJson() const { /* ... */ }
        // data
        std::size_t index = 0;
        bool blocked = false;
        int x = 0;
        int y = 0;
    };

    using GraphT = EvoAI::LatticeGraph<MazeNode>;

The ``x``/``y`` fields are the only reason this example doesn't just use ``NodeDefault``:
:class:`DefaultTraversalPolicy<EvoAI::PathFindingAlgorithms::DefaultTraversalPolicy>` detects
them at compile time and switches its heuristic from an always-``0.0`` stub to real Euclidean
distance. Without them, A* silently degrades into plain Dijkstra and GBFS has no ranking
signal to search by at all — neither would actually be searching *toward* the goal, which
would defeat the point of racing four algorithms with visibly different behaviour.

The maze itself is grown from a fully-connected lattice (``LatticeGraph``'s constructor
*is* that lattice — there's no separate factory call), then carved by
:class:`GrowingTree<EvoAI::MazeAlgorithms::GrowingTree>` starting from the cell at the
center of the grid. ``GrowingTree`` is templated on the graph type and only needs
``forEachOutgoingEdge``/``isolate``, so it runs unmodified on ``LatticeGraph`` exactly as
it does on ``Graph``:

.. code-block:: cpp

    GraphT buildMaze(const SimConfig& cfg) {
        GraphT g(cfg.rows, cfg.cols, false);
        for (std::size_t i = 0; i < g.size(); ++i) {
            g[i].x = static_cast<int>(i % cfg.cols);
            g[i].y = static_cast<int>(i / cfg.cols);
        }
        MazeAlg::GrowingTree<GraphT> growingTree;
        growingTree(g, g[cfg.startIdx], cfg.randomness, cfg.binomial);
        return g;
    }

``randomness`` and ``binomial`` tune the character of the generated maze — left unset on the
command line, they default to two draws from the seeded RNG, so a given ``--seed`` always
reproduces the same maze, the same algorithm assignment, and the same race.

Opening up the start and the targets
--------------------------------------

Growing Tree can wall off the exact cell it started from, and the bottom row will not have a
clear opening anywhere by construction, so the demo manually carves room for both. Unlike
``Graph``, ``LatticeGraph`` has no ``connectToX()``/``connect()`` to call: since every
``forEachOutgoingEdge`` call derives edges from ``.blocked`` and index math fresh, unblocking
a cell is the whole operation — connectivity to any already-open neighbour comes back on its
own, in both directions, with nothing left to reconnect by hand:

.. code-block:: cpp

    void clearCenter(GraphT& g, const SimConfig& cfg) {
        const int rows = static_cast<int>(cfg.rows);
        const int cols = static_cast<int>(cfg.cols);
        for (int dy = -cfg.clearRY; dy <= cfg.clearRY; ++dy) {
            for (int dx = -cfg.clearRX; dx <= cfg.clearRX; ++dx) {
                int r = static_cast<int>(cfg.centerRow) + dy;
                int c = static_cast<int>(cfg.centerCol) + dx;
                if (r < 0 || r >= rows || c < 0 || c >= cols) continue;
                std::size_t idx = static_cast<std::size_t>(r) * cfg.cols
                                + static_cast<std::size_t>(c);
                g[idx].blocked = false;
            }
        }
    }

One target cell per letter is opened the same way along the bottom row, spaced out and
centered under the maze, along with the cell directly above it so a search can actually
arrive into the target from the rest of the maze:

.. code-block:: cpp

    std::vector<std::size_t> setupTargets(
        GraphT& g, const SimConfig& cfg, const std::string& text)
    {
        const int cols = static_cast<int>(cfg.cols);
        const int bottomRow = static_cast<int>(cfg.rows) - 1;
        int firstCol = static_cast<int>(cfg.centerCol) - static_cast<int>(text.size()) / 2;
        if (firstCol < 1) firstCol = 1;

        std::vector<std::size_t> targets;
        for (std::size_t i = 0; i < text.size(); ++i) {
            int c = firstCol + static_cast<int>(i);
            if (c >= cols - 1) c = cols - 2;
            std::size_t idx = static_cast<std::size_t>(bottomRow) * cfg.cols
                            + static_cast<std::size_t>(c);
            g[idx].blocked = false;

            if (bottomRow > 0) {
                std::size_t northIdx = static_cast<std::size_t>(bottomRow - 1) * cfg.cols
                                     + static_cast<std::size_t>(c);
                g[northIdx].blocked = false;
            }
            targets.push_back(idx);
        }
        return targets;
    }

Assigning an algorithm to each letter
----------------------------------------

Every character of the input text gets its own :cpp:struct:`LetterState`: which target it is
walking towards, and which of the four algorithms will compute its route. The algorithm is
picked uniformly at random, independently per letter:

.. code-block:: cpp

    enum class Algorithm { DFS, BFS, GBFS, ASTAR };

    static Algorithm pickRandomAlgorithm(std::mt19937& rng) {
        return static_cast<Algorithm>(std::uniform_int_distribution<int>(0, 3)(rng));
    }

    struct LetterState {
        char                     ch        = '\0';
        std::atomic<std::size_t> currentIdx{0};
        std::size_t              goalIdx   = 0;
        Algorithm                algo      = Algorithm::ASTAR;
        std::atomic<bool>        finished  {false};
    };

Searching concurrently
------------------------

A small thread pool pulls letters one at a time from a ``TaskQueue``. Each worker owns a
single :class:`SearchContext<EvoAI::PathFindingAlgorithms::SearchContext>` and
:class:`DefaultTraversalPolicy<EvoAI::PathFindingAlgorithms::DefaultTraversalPolicy>`,
created once before the task loop and reused for every letter that worker processes, so
every worker reads the shared maze through ``const`` references without needing any locking
around the search itself:

.. code-block:: cpp

    // created once per worker thread, outside the task loop
    PFA::SearchContext ctx;
    PolicyT            policy;

    while (taskQueue.pop(task)) {
        std::vector<const GraphT::Node_t*> path;

        const auto& startNode = graph[ls.currentIdx.load()];
        const auto& goalNode  = graph[ls.goalIdx];

        switch (ls.algo) {
            case Algorithm::DFS:  { PFA::DFS<GraphT,PolicyT>  a; path = a(graph,startNode,goalNode,ctx,policy); break; }
            case Algorithm::BFS:  { PFA::BFS<GraphT,PolicyT>  a; path = a(graph,startNode,goalNode,ctx,policy); break; }
            case Algorithm::GBFS: { PFA::GBFS<GraphT,PolicyT> a; path = a(graph,startNode,goalNode,ctx,policy); break; }
            case Algorithm::ASTAR:{ PFA::AStar<GraphT,PolicyT>a; path = a(graph,startNode,goalNode,ctx,policy); break; }
        }
        // ...
    }

Each algorithm calls ``ctx.reset(graph.size())`` internally before searching. Because
:class:`SearchContext<EvoAI::PathFindingAlgorithms::SearchContext>`'s buffers only grow and
``reset()`` is just a generation-counter bump, reusing one ``ctx`` per worker means only the
first search on each thread pays the cost of growing those buffers -- every later search on
that thread, including across different letters and different algorithms, reuses the same
capacity for free. This is also why sharing one ``ctx`` across DFS/BFS/GBFS/AStar calls on
the same worker is safe: nothing about the buffers is algorithm-specific.

Animating and rendering
--------------------------

Once a path is found, the worker walks it in a small number of evenly spaced frames, storing
each step into the letter's atomic ``currentIdx``:

.. code-block:: cpp

    if (!path.empty()) {
        constexpr std::size_t maxFrames = 40;
        const std::size_t stride = std::max(std::size_t{1}, path.size() / maxFrames);
        for (std::size_t step = 0; step < path.size(); step += stride) {
            ls.currentIdx.store(path[step]->index, std::memory_order_relaxed);
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    }

A separate render loop on the main thread redraws the whole grid at a fixed rate, reading
each letter's ``currentIdx`` to place it on the grid and color it by its assigned algorithm
(cyan for DFS, green for BFS, yellow for GBFS, magenta for A*), so the maze walls, the moving
letters, and the still-empty targets are all visible at once while the searches race to
finish.

The full code is `here`_

.. _here: https://github.com/cristianglezm/EvoAI/blob/master/examples/Maze/
