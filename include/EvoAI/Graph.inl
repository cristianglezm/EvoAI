namespace EvoAI {
    inline NodeDefault::NodeDefault()
    : index(0)
    , blocked(false){}
    inline NodeDefault::NodeDefault(JsonBox::Object o)
    : index(safeParseUInt<std::size_t>(o["index"].getString(), 0))
    , blocked(o["blocked"].getBoolean()){}
    inline JsonBox::Value NodeDefault::toJson() const{
        JsonBox::Object o;
        o["index"] = JsonBox::Value(std::to_string(index));
        o["blocked"] = JsonBox::Value(blocked);
        return JsonBox::Value(o);
    }
    inline bool NodeDefault::operator==(const NodeDefault& rhs) const noexcept{
        return index == rhs.index && blocked == rhs.blocked;
    }
    inline bool NodeDefault::operator!=(const NodeDefault& rhs) const noexcept{
        return !((*this) == rhs);
    }
    inline EdgeDefault::EdgeDefault()
    : weight(0)
    , prev(0)
    , next(0){}
    inline EdgeDefault::EdgeDefault(const std::size_t& p, const std::size_t& n, const int& w)
    : weight(w)
    , prev(p)
    , next(n){}
    inline EdgeDefault::EdgeDefault(JsonBox::Object o)
    : weight(o["weight"].getInteger())
    , prev(safeParseUInt<std::size_t>(o["prev"].getString(), 0))
    , next(safeParseUInt<std::size_t>(o["next"].getString(), 0)){}
    inline JsonBox::Value EdgeDefault::toJson() const{
        JsonBox::Object o;
        o["weight"] = JsonBox::Value(weight);
        o["prev"] = JsonBox::Value(std::to_string(prev));
        o["next"] = JsonBox::Value(std::to_string(next));
        return JsonBox::Value(o);
    }
    inline bool EdgeDefault::operator==(const EdgeDefault& rhs) const noexcept{
        return weight == rhs.weight && prev == rhs.prev && next == rhs.next;
    }
    inline bool EdgeDefault::operator!=(const EdgeDefault& rhs) const noexcept{
        return !((*this) == rhs);
    }
    inline bool EdgeDefault::operator<=(const EdgeDefault& rhs) const noexcept{
        return weight <= rhs.weight;
    }
    template<typename NodeType, typename EdgeType>
    Graph<NodeType, EdgeType>::Graph()
    : nodes()
    , edges()
    , head()
    , row(0)
    , col(0){}
    template<typename NodeType, typename EdgeType>
    Graph<NodeType, EdgeType>::Graph(const std::size_t& row_, const std::size_t& col_)
    : nodes()
    , edges()
    , head()
    , row(row_)
    , col(col_){
        nodes.reserve(row_ * col_);
        head.reserve(row_ * col_);
        for(std::size_t n = 0; n < (row_ * col_); ++n) {
            addNode(Node_t());
        }
    }
    template<typename NodeType, typename EdgeType>
    Graph<NodeType, EdgeType>::Graph(JsonBox::Object o)
    : nodes()
    , edges()
    , head()
    , row(0)
    , col(0){
        row = safeParseUInt<std::size_t>(o["row"].getString(), 0);
        col = safeParseUInt<std::size_t>(o["col"].getString(), 0);
        
        auto& nArray = o["nodes"].getArray();
        std::size_t numNodes = nArray.size();
        nodes.reserve(numNodes);
        head.assign(numNodes, npos);

        for(auto& n : nArray) {
            nodes.emplace_back(n.getObject());
            nodes.back().index = nodes.size() - 1;
        }

        // edges are stored as array-of-arrays in JSON (legacy). We reconstruct forward-star.
        auto& vecArray = o["edges"].getArray();
        for(std::size_t i = 0; i < vecArray.size(); ++i) {
            auto& eArray = vecArray[i].getArray();
            for(auto& e : eArray) {
                Edge_t et(e.getObject());
                if(et.prev >= numNodes || et.next >= numNodes){
                    throw std::out_of_range("Graph(JsonBox::Object) - edge references a node index that doesn't exist");
                }
                addDirectedEdge(et);
            }
        }
    }
    template<typename NodeType, typename EdgeType>
    JsonBox::Value Graph<NodeType, EdgeType>::toJson() const{
        JsonBox::Object o;
        JsonBox::Array nArray;
        for(auto const& n : nodes) {
            nArray.push_back(n.toJson());
        }

        // Convert forward-star back to vector<vector<Edge>> for JSON compatibility
        JsonBox::Array vecArray;
        vecArray.resize(nodes.size());
        for(std::size_t u = 0; u < nodes.size(); ++u) {
            JsonBox::Array eArray;
            forEachOutgoingEdge(nodes[u], [&](const Edge_t& e){
                eArray.push_back(e.toJson());
            });
            vecArray[u] = JsonBox::Array(eArray);
        }

        o["row"] = JsonBox::Value(std::to_string(row));
        o["col"] = JsonBox::Value(std::to_string(col));
        o["nodes"] = JsonBox::Value(nArray);
        o["edges"] = JsonBox::Value(vecArray);
        return JsonBox::Value(o);
    }
    template<typename NodeType, typename EdgeType>
    void Graph<NodeType, EdgeType>::writeToFile(const std::string& filename) const{
        JsonBox::Value v;
        v["version"] = JsonBox::Value("1.0");
        v["graph"] = toJson();
        v.writeToFile(filename);
    }
    template<typename NodeType, typename EdgeType>
    void Graph<NodeType, EdgeType>::addNode(Node_t n){
        n.index = nodes.size();
        nodes.emplace_back(n);
        head.emplace_back(npos);
    }
    template<typename NodeType, typename EdgeType>
    typename Graph<NodeType, EdgeType>::Node_t& Graph<NodeType, EdgeType>::getNode(const std::size_t& x, const std::size_t& y){
        if(row == 0 || col == 0) {
            throw std::out_of_range("Graph::getNode - You need to update your row or col");
        }
        auto index = y + col * x;
        return nodes[index];
    }
    template<typename NodeType, typename EdgeType>
    std::vector<typename Graph<NodeType, EdgeType>::Node_t*> Graph<NodeType, EdgeType>::getNeighbours(const Node_t& n){
        std::vector<Node_t*> neighbours;
        std::size_t ei = head[n.index];
        while(ei != npos) {
            neighbours.emplace_back(&nodes[edges[ei].payload.next]);
            ei = edges[ei].nextIndex;
        }
        return neighbours;
    }
    template<typename NodeType, typename EdgeType>
    std::pair<std::size_t,std::size_t> Graph<NodeType, EdgeType>::getIndex2d(const Node_t& n){
        if(row == 0 || col == 0) {
            throw std::out_of_range("Graph::getIndex2d - You need to update your row or col");
        }
        std::size_t r = n.index / col;
        std::size_t c = n.index % col;
        return std::make_pair(r,c);
    }
    template<typename NodeType, typename EdgeType>
    void Graph<NodeType, EdgeType>::removeNode(const Node_t& n){
        isolate(n);
    }
    template<typename NodeType, typename EdgeType>
    void Graph<NodeType, EdgeType>::addDirectedEdge(const Edge_t& e){
        // push new internal edge and link at head[e.prev]
        InternalEdge ie;
        ie.payload = e;
        ie.nextIndex = head[e.prev];
        edges.emplace_back(std::move(ie));
        head[e.prev] = edges.size() - 1;
    }
    template<typename NodeType, typename EdgeType>
    void Graph<NodeType, EdgeType>::addUndirectedEdge(const Edge_t& e){
        addDirectedEdge(e);
        Edge_t rev = e;
        rev.prev = e.next;
        rev.next = e.prev;
        addDirectedEdge(rev);
    }
    template<typename NodeType, typename EdgeType>
    void Graph<NodeType, EdgeType>::removeDirectedEdge(const Edge_t& e){
        // Walk linked list at head[e.prev] and remove matching payload (prev==e.prev && next==e.next)
        std::size_t cur = head[e.prev];
        std::size_t prevIdx = npos;
        while(cur != npos) {
            const auto& payload = edges[cur].payload;
            if(payload.prev == e.prev && payload.next == e.next) {
                // remove cur from linked list
                if(prevIdx == npos) {
                    head[e.prev] = edges[cur].nextIndex;
                } else {
                    edges[prevIdx].nextIndex = edges[cur].nextIndex;
                }
                // remove element cur from edges vector by swapping last element into cur
                std::size_t last = edges.size() - 1;
                if(cur != last) {
                    edges[cur] = std::move(edges[last]);
                    // fix any head pointers that pointed to last
                    for(std::size_t u = 0; u < head.size(); ++u) {
                        if(head[u] == last) head[u] = cur;
                    }
                    // fix any nextIndex that pointed to last
                    for(auto &ie : edges) {
                        if(ie.nextIndex == last) ie.nextIndex = cur;
                    }
                }
                edges.pop_back();
                return;
            }
            prevIdx = cur;
            cur = edges[cur].nextIndex;
        }
    }
    template<typename NodeType, typename EdgeType>
    void Graph<NodeType, EdgeType>::removeUndirectedEdge(const Edge_t& e){
        Edge_t rev = e;
        rev.prev = e.next;
        rev.next = e.prev;
        removeDirectedEdge(rev);
        removeDirectedEdge(e);
    }
    template<typename NodeType, typename EdgeType>
    void Graph<NodeType, EdgeType>::removeAllEdges(const Node_t& n){
        // remove all outgoing edges from n
        std::size_t ei = head[n.index];
        while(ei != npos) {
            Edge_t payload = edges[ei].payload;
            removeDirectedEdge(payload);
            ei = head[n.index];
        }
        // remove incoming edges to n
        for(std::size_t u = 0; u < nodes.size(); ++u) {
            if(u == n.index) continue;
            std::size_t cur = head[u];
            while(cur != npos) {
                const auto& payload = edges[cur].payload;
                if(payload.next == n.index) {
                    removeDirectedEdge(payload);
                    // restart scanning u's list because indices may have changed
                    cur = head[u];
                } else {
                    cur = edges[cur].nextIndex;
                }
            }
        }
    }
    template<typename NodeType, typename EdgeType>
    void Graph<NodeType, EdgeType>::isolate(const Node_t& n){
        removeAllEdges(n);
    }
    template<typename NodeType, typename EdgeType>
    bool Graph<NodeType, EdgeType>::areConnected(const Node_t& n1, const Node_t& n2) const{
        bool n1Connected = false;
        std::size_t ei = head[n1.index];
        while(ei != npos) {
            const auto& p = edges[ei].payload;
            if(p.prev == n1.index && p.next == n2.index) { n1Connected = true; break; }
            ei = edges[ei].nextIndex;
        }
        bool n2Connected = false;
        ei = head[n2.index];
        while(ei != npos) {
            const auto& p = edges[ei].payload;
            if(p.prev == n2.index && p.next == n1.index) { n2Connected = true; break; }
            ei = edges[ei].nextIndex;
        }
        return (n1Connected && n2Connected);
    }
    template<typename NodeType, typename EdgeType>
    bool Graph<NodeType, EdgeType>::isConnected(const Node_t& n1, const Node_t& n2) const{
        std::size_t ei = head[n1.index];
        while(ei != npos) {
            const auto& p = edges[ei].payload;
            if(p.prev == n1.index && p.next == n2.index) return true;
            ei = edges[ei].nextIndex;
        }
        return false;
    }
    template<typename NodeType, typename EdgeType>
    void Graph<NodeType, EdgeType>::setRow(const std::size_t& r){
        row = r;
    }
    template<typename NodeType, typename EdgeType>
    const std::size_t& Graph<NodeType, EdgeType>::getRow() const{
        return row;
    }
    template<typename NodeType, typename EdgeType>
    void Graph<NodeType, EdgeType>::setCol(const std::size_t& c){
        col = c;
    }
    template<typename NodeType, typename EdgeType>
    const std::size_t& Graph<NodeType, EdgeType>::getCol() const{
        return col;
    }
    template<typename NodeType, typename EdgeType>
    std::size_t Graph<NodeType, EdgeType>::size() const noexcept{
        return nodes.size();
    }
    template<typename NodeType, typename EdgeType>
    typename Graph<NodeType, EdgeType>::Node_t& Graph<NodeType, EdgeType>::operator[](const std::size_t& index){
        return nodes[index];
    }
    template<typename NodeType, typename EdgeType>
    const typename Graph<NodeType, EdgeType>::Node_t& Graph<NodeType, EdgeType>::operator[](const std::size_t& index) const{
        return nodes[index];
    }
    template<typename NodeType, typename EdgeType>
    const std::vector<EdgeType> Graph<NodeType, EdgeType>::getOutgoingEdges(const Node_t& n) const{
        std::vector<Edge_t> out;
        forEachOutgoingEdge(n, [&](const Edge_t& e){ out.emplace_back(e); });
        return out;
    }
    template<typename NodeType, typename EdgeType>
    template<typename Callback>
    void Graph<NodeType, EdgeType>::forEachOutgoingEdge(const Node_t& n, Callback&& callback) const{
        std::size_t ei = head[n.index];
        while(ei != npos){
            callback(edges[ei].payload);
            ei = edges[ei].nextIndex;
        }
    }
    template<typename NodeType, typename EdgeType>
    bool Graph<NodeType, EdgeType>::operator==(const Graph& rhs) const{
        if(nodes != rhs.nodes) return false;
        if(row != rhs.row || col != rhs.col) return false;
        auto sortEdges = [](std::vector<Edge_t> v) {
            std::sort(v.begin(), v.end(), [](const Edge_t& a, const Edge_t& b) {
                if(a.prev != b.prev) return a.prev < b.prev;
                if(a.next != b.next) return a.next < b.next;
                return a.weight < b.weight;
            });
            return v;
        };
        for(std::size_t u = 0; u < nodes.size(); ++u) {
            std::vector<Edge_t> lhsEdges, rhsEdges;
            forEachOutgoingEdge(nodes[u], [&](const Edge_t& e){ lhsEdges.emplace_back(e); });
            rhs.forEachOutgoingEdge(rhs.nodes[u], [&](const Edge_t& e){ rhsEdges.emplace_back(e); });
            if(sortEdges(std::move(lhsEdges)) != sortEdges(std::move(rhsEdges)))
                return false;
        }
        return true;
    }
    template<typename NodeType, typename EdgeType>
    bool Graph<NodeType, EdgeType>::operator!=(const Graph& rhs) const{
        return !((*this) == rhs);
    }
    template<typename NodeType, typename EdgeType>
    void Graph<NodeType, EdgeType>::connect(Node_t& n1, Node_t& n2, int weight) noexcept{
        if(!isConnected(n1,n2)) {
            Edge_t e;
            e.prev = n1.index;
            e.next = n2.index;
            e.weight = weight;
            addDirectedEdge(e);
        }
    }
    template<typename NodeType, typename EdgeType>
    void Graph<NodeType, EdgeType>::connectToEachOther(Node_t& n1, Node_t& n2, int weight) noexcept{
        connect(n1, n2, weight);
        connect(n2, n1, weight);
    }
    template<typename NodeType, typename EdgeType>
    void Graph<NodeType, EdgeType>::disconnect(Node_t& n1, Node_t& n2) noexcept{
        if(isConnected(n1,n2)) {
            Edge_t e;
            e.prev = n1.index;
            e.next = n2.index;
            e.weight = 0;
            removeDirectedEdge(e);
        }
    }
    template<typename NodeType, typename EdgeType>
    void Graph<NodeType, EdgeType>::disconnectFromEachOther(Node_t& n1, Node_t& n2) noexcept{
        if(areConnected(n1,n2)) {
            Edge_t e;
            e.prev = n1.index;
            e.next = n2.index;
            e.weight = 0;
            removeUndirectedEdge(e);
        }
    }
    template<typename NodeType, typename EdgeType>
    void Graph<NodeType, EdgeType>::connectToNorth(Node_t& n, int weight) noexcept{
        auto nb = getNorth(n); if(nb) connect(n,*nb,weight);
    }
    template<typename NodeType, typename EdgeType>
    void Graph<NodeType, EdgeType>::connectToNorthEast(Node_t& n, int weight) noexcept{
        auto nb = getNorthEast(n); if(nb) connect(n,*nb,weight);
    }
    template<typename NodeType, typename EdgeType>
    void Graph<NodeType, EdgeType>::connectToNorthWest(Node_t& n, int weight) noexcept{
        auto nb = getNorthWest(n); if(nb) connect(n,*nb,weight);
    }
    template<typename NodeType, typename EdgeType>
    void Graph<NodeType, EdgeType>::connectToEast(Node_t& n, int weight) noexcept{
        auto nb = getEast(n); if(nb) connect(n,*nb,weight);
    }
    template<typename NodeType, typename EdgeType>
    void Graph<NodeType, EdgeType>::connectToWest(Node_t& n, int weight) noexcept{
        auto nb = getWest(n); if(nb) connect(n,*nb,weight);
    }
    template<typename NodeType, typename EdgeType>
    void Graph<NodeType, EdgeType>::connectToSouth(Node_t& n, int weight) noexcept{
        auto nb = getSouth(n); if(nb) connect(n,*nb,weight);
    }
    template<typename NodeType, typename EdgeType>
    void Graph<NodeType, EdgeType>::connectToSouthEast(Node_t& n, int weight) noexcept{
        auto nb = getSouthEast(n); if(nb) connect(n,*nb,weight);
    }
    template<typename NodeType, typename EdgeType>
    void Graph<NodeType, EdgeType>::connectToSouthWest(Node_t& n, int weight) noexcept{
        auto nb = getSouthWest(n); if(nb) connect(n,*nb,weight);
    }
    template<typename NodeType, typename EdgeType>
    typename Graph<NodeType, EdgeType>::Node_t* Graph<NodeType, EdgeType>::getNorth(Node_t& n) noexcept{
        if(n.index < col) return nullptr;
        auto neighbour = n.index - col;
        if(neighbour < size()) return &nodes[neighbour];
        return nullptr;
    }
    template<typename NodeType, typename EdgeType>
    typename Graph<NodeType, EdgeType>::Node_t* Graph<NodeType, EdgeType>::getNorthEast(Node_t& n) noexcept{
        if(n.index < col) return nullptr;
        auto neighbour = n.index - col + 1;
        std::size_t currentCol = n.index % col;
        if(currentCol != (col - 1) && neighbour < size()) return &nodes[neighbour];
        return nullptr;
    }
    template<typename NodeType, typename EdgeType>
    typename Graph<NodeType, EdgeType>::Node_t* Graph<NodeType, EdgeType>::getNorthWest(Node_t& n) noexcept{
        if(n.index < col) return nullptr;
        auto neighbour = n.index - col - 1;
        std::size_t currentCol = n.index % col;
        if(currentCol != 0u && neighbour < size()) return &nodes[neighbour];
        return nullptr;
    }
    template<typename NodeType, typename EdgeType>
    typename Graph<NodeType, EdgeType>::Node_t* Graph<NodeType, EdgeType>::getEast(Node_t& n) noexcept{
        auto neighbour = n.index + 1;
        std::size_t currentCol = n.index % col;
        if(currentCol != (col - 1) && neighbour < size()) return &nodes[neighbour];
        return nullptr;
    }
    template<typename NodeType, typename EdgeType>
    typename Graph<NodeType, EdgeType>::Node_t* Graph<NodeType, EdgeType>::getWest(Node_t& n) noexcept{
        if(n.index == 0u) return nullptr;
        auto neighbour = n.index - 1;
        std::size_t currentCol = n.index % col;
        if(currentCol != 0u && neighbour < size()) return &nodes[neighbour];
        return nullptr;
    }
    template<typename NodeType, typename EdgeType>
    typename Graph<NodeType, EdgeType>::Node_t* Graph<NodeType, EdgeType>::getSouth(Node_t& n) noexcept{
        auto neighbour = n.index + col;
        if(neighbour < size()) return &nodes[neighbour];
        return nullptr;
    }
    template<typename NodeType, typename EdgeType>
    typename Graph<NodeType, EdgeType>::Node_t* Graph<NodeType, EdgeType>::getSouthEast(Node_t& n) noexcept{
        auto neighbour = n.index + col + 1;
        std::size_t currentCol = n.index % col;
        if(currentCol != (col - 1) && neighbour < size()) return &nodes[neighbour];
        return nullptr;
    }
    template<typename NodeType, typename EdgeType>
    typename Graph<NodeType, EdgeType>::Node_t* Graph<NodeType, EdgeType>::getSouthWest(Node_t& n) noexcept{
        auto neighbour = n.index + col - 1;
        std::size_t currentCol = n.index % col;
        if(currentCol != 0u && neighbour < size()) return &nodes[neighbour];
        return nullptr;
    }
    template<typename NodeType, typename EdgeType>
    std::vector<typename Graph<NodeType, EdgeType>::Node_t*> Graph<NodeType, EdgeType>::getAllNeighbours(Node_t& n, bool diagonal) noexcept{
        std::vector<Node_t*> neighbours;
        if(auto nb = getNorth(n)) neighbours.emplace_back(nb);
        if(auto nb = getEast(n)) neighbours.emplace_back(nb);
        if(auto nb = getSouth(n)) neighbours.emplace_back(nb);
        if(auto nb = getWest(n)) neighbours.emplace_back(nb);
        if(diagonal) {
            if(auto nb = getNorthEast(n)) neighbours.emplace_back(nb);
            if(auto nb = getNorthWest(n)) neighbours.emplace_back(nb);
            if(auto nb = getSouthEast(n)) neighbours.emplace_back(nb);
            if(auto nb = getSouthWest(n)) neighbours.emplace_back(nb);
        }
        return neighbours;
    }
    template<typename NodeType, typename EdgeType>
    void Graph<NodeType, EdgeType>::connectToNeighbours(Node_t& n, int weight, bool diagonal) noexcept{
        connectToNorth(n, weight);
        connectToEast(n, weight);
        connectToSouth(n, weight);
        connectToWest(n, weight);
        if(diagonal) {
            connectToNorthEast(n, weight);
            connectToNorthWest(n, weight);
            connectToSouthEast(n, weight);
            connectToSouthWest(n, weight);
        }
    }
    template<typename NodeType, typename EdgeType>
    Graph<NodeType, EdgeType> Graph<NodeType, EdgeType>::createLatticeGraph(const std::size_t& row_, const std::size_t& col_, bool diagonal){
        Graph g(row_, col_);
        for(auto& n : g.nodes) {
            g.connectToNeighbours(n, 0, diagonal);
        }
        return g;
    }
} // namespace EvoAI
