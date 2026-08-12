// libFuzzer harness for EvoAI::Graph's JSON constructor (used by
// LatticeGraph and the maze/pathfinding algorithms built on top of it).
//
// The bug this harness exists to guard didn't
// crash at construction: an edge with an out-of-range node index built a
// structurally corrupt but successfully-constructed Graph, and only
// crashed later, the first time something dereferenced a neighbour
// through that edge. So construction alone isn't enough exercise here -
// this harness has to actually walk the graph's edges and dereference
// each neighbour the way a real consumer (e.g. pathfinding) would.
#include <EvoAI/Graph.hpp>
#include <JsonBox.h>
#include <cstddef>
#include <cstdint>
#include <string>

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size){
    std::string input(reinterpret_cast<const char*>(data), size);
    JsonBox::Value v;
    try{
        v.loadFromString(input);
    }catch(const std::exception&){
        return 0;
    }
    if(!v.isObject()){
        return 0;
    }
    try{
        EvoAI::Graph<> g(v.getObject());
        for(std::size_t i = 0; i < g.size(); ++i){
            g.forEachOutgoingEdge(g[i], [&](const EvoAI::EdgeDefault& e){
                // Dereference the neighbour the way BFS/DFS/A* do -
                // this is what actually reads out-of-bounds if an
                // edge's node index doesn't exist.
                (void)g[e.next];
            });
        }
    }catch(const std::exception&){
        return 0;
    }
    return 0;
}
