#include <EvoAI/Genome.hpp>

#include <random>
#include <algorithm>
//#include <execution>
#include <cassert>
#include <future>
#include <EvoAI/NeuronLayer.hpp>

namespace EvoAI{
    Genome::Genome()
    : genomeID(0)
    , speciesID(0)
    , fitness(0.0)
    , rnnAllowed(false)
    , cppn(false)
    , nodeChromosomes()
    , connectionChromosomes(){}
    Genome::Genome(const Genome& rhs) noexcept
    : genomeID(rhs.genomeID)
    , speciesID(rhs.speciesID)
    , fitness(rhs.fitness)
    , rnnAllowed(rhs.rnnAllowed)
    , cppn(rhs.cppn)
    , nodeChromosomes(rhs.nodeChromosomes)
    , connectionChromosomes(rhs.connectionChromosomes){
        std::sort(std::begin(nodeChromosomes), std::end(nodeChromosomes));
        std::sort(std::begin(connectionChromosomes), std::end(connectionChromosomes));
    }
    Genome::Genome(Genome&& rhs) noexcept
    : genomeID(rhs.genomeID)
    , speciesID(rhs.speciesID)
    , fitness(rhs.fitness)
    , rnnAllowed(rhs.rnnAllowed)
    , cppn(rhs.cppn)
    , nodeChromosomes(std::move(rhs.nodeChromosomes))
    , connectionChromosomes(std::move(rhs.connectionChromosomes)){
        std::sort(std::begin(nodeChromosomes), std::end(nodeChromosomes));
        std::sort(std::begin(connectionChromosomes), std::end(connectionChromosomes));
    }
    Genome::Genome(const std::size_t& numInputs, const std::size_t& numOutputs, bool canBeRecursive, bool cppn)
    : genomeID(0)
    , speciesID(0)
    , fitness(0.0)
    , rnnAllowed(canBeRecursive)
    , cppn(cppn)
    , nodeChromosomes()
    , connectionChromosomes(){
        nodeChromosomes.reserve(numInputs + numOutputs);
        for(auto i=0u;i<numInputs;++i){
            nodeChromosomes.emplace_back(0,i,Neuron::Type::INPUT,Neuron::ActivationType::SIGMOID);
        }
        for(auto i=0u;i<numOutputs;++i){
            Neuron::ActivationType at = Neuron::ActivationType::SIGMOID;
            if(cppn){
                at = Genome::getRandomActivationType();
            }
            nodeChromosomes.emplace_back(2,i,Neuron::Type::OUTPUT,at);
        }
        connectionChromosomes.reserve(numInputs * numOutputs);
        for(auto i=0u;i<numInputs;++i){
            for(auto j=0u;j<numOutputs;++j){
                connectionChromosomes.emplace_back(NodeGene(0,i), NodeGene(2,j), randomGen.random(-1.0,1.0, numInputs + numOutputs));
            }
        }
        std::sort(std::begin(nodeChromosomes), std::end(nodeChromosomes));
        std::sort(std::begin(connectionChromosomes), std::end(connectionChromosomes));
    }
    Genome::Genome(const std::size_t& numInputs, const std::size_t& numHidden, const std::size_t& numOutputs, bool canBeRecursive, bool cppn)
    : genomeID(0)
    , speciesID(0)
    , fitness(0.0)
    , rnnAllowed(canBeRecursive)
    , cppn(cppn)
    , nodeChromosomes()
    , connectionChromosomes(){
        nodeChromosomes.reserve(numInputs + numHidden + numOutputs);
        for(auto i=0u;i<numInputs;++i){
            nodeChromosomes.emplace_back(0,i,Neuron::Type::INPUT,Neuron::ActivationType::SIGMOID);
        }
        for(auto i=0u;i<numHidden;++i){
            Neuron::ActivationType at = Neuron::ActivationType::SIGMOID;
            if(cppn){
                at = Genome::getRandomActivationType();
            }
            nodeChromosomes.emplace_back(1,i,Neuron::Type::HIDDEN,at);
        }
        for(auto i=0u;i<numOutputs;++i){
            Neuron::ActivationType at = Neuron::ActivationType::SIGMOID;
            if(cppn){
                at = Genome::getRandomActivationType();
            }
            nodeChromosomes.emplace_back(2,i,Neuron::Type::OUTPUT,at);
        }
        connectionChromosomes.reserve((numInputs * numHidden) + (numHidden * numOutputs));
        for(auto i=0u;i<numInputs;++i){
            for(auto j=0u;j<numHidden;++j){
                connectionChromosomes.emplace_back(NodeGene(0,i), NodeGene(1,j), randomGen.random(-1.0,1.0,numInputs + numHidden));
            }
        }
        for(auto i=0u;i<numHidden;++i){
            for(auto j=0u;j<numOutputs;++j){
                connectionChromosomes.emplace_back(NodeGene(1,i), NodeGene(2,j), randomGen.random(-1.0,1.0,numHidden + numOutputs));
            }
        }
        std::sort(std::begin(nodeChromosomes), std::end(nodeChromosomes));
        std::sort(std::begin(connectionChromosomes), std::end(connectionChromosomes));
    }
    Genome::Genome(JsonBox::Object o)
    : genomeID(std::stoull(o["GenomeID"].getString()))
    , speciesID(std::stoull(o["SpeciesID"].getString()))
    , fitness(o["fitness"].getDouble())
    , rnnAllowed(o["rnnAllowed"].getBoolean())
    , cppn(o["cppn"].getBoolean())
    , nodeChromosomes()
    , connectionChromosomes(){
        auto& ngs = o["nodeChromosomes"].getArray();
        nodeChromosomes.reserve(ngs.size());
        for(auto& ng:ngs){
            nodeChromosomes.emplace_back(ng.getObject());
        }
        auto& cgs = o["ConnectionChromosomes"].getArray();
        connectionChromosomes.reserve(cgs.size());
        for(auto& cg:cgs){
            connectionChromosomes.emplace_back(cg.getObject());
        }
        std::sort(std::begin(nodeChromosomes), std::end(nodeChromosomes));
        std::sort(std::begin(connectionChromosomes), std::end(connectionChromosomes));
    }
    Genome::Genome(const std::string& jsonfile)
    : genomeID(0)
    , speciesID(0)
    , fitness(0.0)
    , rnnAllowed(false)
    , cppn(false)
    , nodeChromosomes()
    , connectionChromosomes(){
        JsonBox::Value json;
        json.loadFromFile(jsonfile);
        auto& v = json["Genome"];
        rnnAllowed = v["rnnAllowed"].getBoolean();
        cppn = v["cppn"].getBoolean();
        genomeID = std::stoull(v["GenomeID"].getString());
        speciesID = std::stoull(v["SpeciesID"].getString());
        fitness = v["fitness"].getDouble();
        auto& ngs = v["nodeChromosomes"].getArray();
        nodeChromosomes.reserve(ngs.size());
        for(auto& ng:ngs){
            nodeChromosomes.emplace_back(ng.getObject());
        }
        auto& cgs = v["ConnectionChromosomes"].getArray();
        connectionChromosomes.reserve(cgs.size());
        for(auto& cg:cgs){
            connectionChromosomes.emplace_back(cg.getObject());
        }
        std::sort(std::begin(nodeChromosomes), std::end(nodeChromosomes));
        std::sort(std::begin(connectionChromosomes), std::end(connectionChromosomes));
    }
    void Genome::addGene(const NodeGene& ng) noexcept{
        nodeChromosomes.emplace_back(ng);
        std::sort(std::begin(nodeChromosomes), std::end(nodeChromosomes));
    }
    void Genome::addGene(const ConnectionGene& cg) noexcept{
        connectionChromosomes.emplace_back(cg);
        std::sort(std::begin(connectionChromosomes), std::end(connectionChromosomes));
    }
    void Genome::setNodeChromosomes(std::vector<NodeGene>&& ngenes) noexcept{
        nodeChromosomes = std::move(ngenes);
        std::sort(std::begin(nodeChromosomes), std::end(nodeChromosomes));
    }
    std::vector<NodeGene>& Genome::getNodeChromosomes() noexcept{
        return nodeChromosomes;
    }
    const std::vector<NodeGene>& Genome::getNodeChromosomes() const noexcept{
        return nodeChromosomes;
    }
    void Genome::setConnectionChromosomes(std::vector<ConnectionGene>&& cgenes) noexcept{
        connectionChromosomes = std::move(cgenes);
        std::sort(std::begin(connectionChromosomes), std::end(connectionChromosomes));
    }
    std::vector<ConnectionGene>& Genome::getConnectionChromosomes() noexcept{
        return connectionChromosomes;
    }
    const std::vector<ConnectionGene>& Genome::getConnectionChromosomes() const noexcept{
        return connectionChromosomes;
    }
    std::size_t Genome::getNumOfNodes(const std::size_t& layerID) const noexcept{
        return std::count_if(std::begin(nodeChromosomes), std::end(nodeChromosomes), 
            [&layerID](const auto& n){
               return n.getLayerID() == layerID;
        });
    }
    JsonBox::Value Genome::toJson() const noexcept{
        JsonBox::Object o;
        o["GenomeID"] = JsonBox::Value(std::to_string(genomeID));
        o["SpeciesID"] = JsonBox::Value(std::to_string(speciesID));
        o["fitness"] = JsonBox::Value(fitness);
        o["cppn"] = JsonBox::Value(cppn);
        o["rnnAllowed"] = JsonBox::Value(rnnAllowed);
        JsonBox::Array nChromo;
        nChromo.reserve(nodeChromosomes.size());
        for(auto& n:nodeChromosomes){
            nChromo.emplace_back(n.toJson());
        }
        JsonBox::Array cChromo;
        cChromo.reserve(connectionChromosomes.size());
        for(auto& c:connectionChromosomes){
            cChromo.emplace_back(c.toJson());
        }
        o["nodeChromosomes"] = JsonBox::Value(nChromo);
        o["ConnectionChromosomes"] = JsonBox::Value(cChromo);
        return JsonBox::Value(o);
    }
    void Genome::writeToFile(const std::string& filename) const noexcept{
        JsonBox::Value v;
        v["version"] = JsonBox::Value("1.0");
        v["Genome"] = toJson();
        v.writeToFile(filename);
    }
    void Genome::setFitness(const double& fit) noexcept{
        fitness = fit;
    }
    void Genome::addFitness(const double& amount) noexcept{
        fitness += amount;
    }
    const double& Genome::getFitness() const noexcept{
        return fitness;
    }
    bool Genome::hasNodeGene(const NodeGene& ng) const noexcept{
        return std::binary_search(std::begin(nodeChromosomes), std::end(nodeChromosomes), ng);
    }
    bool Genome::hasConnectionGene(const ConnectionGene& cg) const noexcept{
        return std::binary_search(std::begin(connectionChromosomes), std::end(connectionChromosomes), cg);
    }
    void Genome::setGenomeID(const std::size_t& gnmID) noexcept{
        genomeID = gnmID;
    }
    const std::size_t& Genome::getGenomeID() const noexcept{
        return genomeID;
    }
    void Genome::setSpeciesID(const std::size_t& spcID) noexcept{
        speciesID = spcID;
    }
    const std::size_t& Genome::getSpeciesID() const noexcept{
        return speciesID;
    }
    void Genome::setCppn(bool isCppn) noexcept{
        cppn = isCppn;
    }
    bool Genome::isCppn() const noexcept{
        return cppn;
    }
    void Genome::mutateAddNode() noexcept{
        if(!connectionChromosomes.empty()){
            auto selectedConnection = randomGen.random(0u,connectionChromosomes.size()-1);
            auto& selConn = connectionChromosomes[selectedConnection];
            auto at = Neuron::ActivationType::SIGMOID;
            if(cppn){
                at = getRandomActivationType();
            }
            NodeGene ng(1,getNumOfNodes(1),Neuron::Type::HIDDEN,at);
            nodeChromosomes.emplace_back(ng);
            selConn.setEnabled(false);
            ConnectionGene cg1(NodeGene(selConn.getSrc().layer,selConn.getSrc().neuron), ng, 1.0);
            ConnectionGene cg2(ng,NodeGene(selConn.getDest().layer,selConn.getDest().neuron), selConn.getWeight());
            connectionChromosomes.emplace_back(cg1);
            connectionChromosomes.emplace_back(cg2);
        }
        std::sort(std::begin(nodeChromosomes), std::end(nodeChromosomes));
        std::sort(std::begin(connectionChromosomes), std::end(connectionChromosomes));
    }
    void Genome::mutateAddConnection() noexcept{
        if(!nodeChromosomes.empty()){
            auto selectedNode1 = randomGen.random(0u,nodeChromosomes.size()-1);
            auto selectedNode2 = randomGen.random(0u,nodeChromosomes.size()-1);
            if(!rnnAllowed){
                if(nodeChromosomes[selectedNode1].getLayerID() < nodeChromosomes[selectedNode2].getLayerID()){
                    connectionChromosomes.emplace_back(nodeChromosomes[selectedNode1], nodeChromosomes[selectedNode2], randomGen.random(-1.0,1.0,static_cast<double>(nodeChromosomes.size())));
                }else{
                    connectionChromosomes.emplace_back(nodeChromosomes[selectedNode2], nodeChromosomes[selectedNode1], randomGen.random(-1.0,1.0,static_cast<double>(nodeChromosomes.size())));
                }
            }else{
                connectionChromosomes.emplace_back(nodeChromosomes[selectedNode1], nodeChromosomes[selectedNode2], randomGen.random(-1.0,1.0,static_cast<double>(nodeChromosomes.size())));
            }
            std::sort(std::begin(connectionChromosomes), std::end(connectionChromosomes));
        }
    }
    void Genome::mutateRemoveConnection() noexcept{
        if(!connectionChromosomes.empty()){
            auto selectedConn = randomGen.random(0u,connectionChromosomes.size()-1);
            connectionChromosomes.erase(std::remove(std::begin(connectionChromosomes),
                                                std::end(connectionChromosomes),
                                                connectionChromosomes[selectedConn]),
                                                std::end(connectionChromosomes));
            std::sort(std::begin(connectionChromosomes), std::end(connectionChromosomes));
        }
    }
    void Genome::mutateWeights(double power) noexcept{
        auto nodeOrConn = randomGen.random(0.5);
        auto shakeThingsUp = randomGen.random(0.5);
        auto isNegative = randomGen.random(0.5);
        if(nodeOrConn){
            if(nodeChromosomes.empty()){
                return;
            }
            auto selectedNode = randomGen.random(0u,nodeChromosomes.size() - 1);
            auto isOld = ((static_cast<std::size_t>(selectedNode)) < (nodeChromosomes.size() / 2));
            if(isOld){
                power += (power * power) * 0.8;
            }
            auto weight = power * randomGen.random(-1.0,1.0,static_cast<double>(nodeChromosomes.size()));
            if(isNegative){
                weight = -weight;
            }
            if(shakeThingsUp){
                nodeChromosomes[selectedNode].setBias(weight);
            }else{
                nodeChromosomes[selectedNode].addBias(weight);
            }
        }else{
            if(connectionChromosomes.empty()){
                return;
            }
            auto selectedConnection = randomGen.random(0u,connectionChromosomes.size() - 1);
            if(connectionChromosomes[selectedConnection].isFrozen()){
                return;
            }
            auto isOld = ((static_cast<std::size_t>(selectedConnection)) < (connectionChromosomes.size() / 2));
            if(isOld){
                power += (power*power) * 0.8;
            }
            auto weight = power * randomGen.random(-1.0,1.0,static_cast<double>(connectionChromosomes.size()));
            if(isNegative){
                weight = -weight;
            }
            if(shakeThingsUp){
                connectionChromosomes[selectedConnection].setWeight(weight);
            }else{
                connectionChromosomes[selectedConnection].addWeight(weight);
            }
        }
    }
    void Genome::mutateDisable() noexcept{
        std::vector<ConnectionGene*> cgs;
        cgs.reserve(connectionChromosomes.size());
        for(auto& c:connectionChromosomes){
            if(c.isEnabled()){
                cgs.emplace_back(&c);
            }
        }
        if(!cgs.empty()){
            cgs[randomGen.random(0u,cgs.size()-1)]->setEnabled(false);
        }
    }
    void Genome::mutateEnable() noexcept{
        std::vector<ConnectionGene*> cgs;
        cgs.reserve(connectionChromosomes.size());
        for(auto& c:connectionChromosomes){
            if(!c.isEnabled()){
                cgs.emplace_back(&c);
            }
        }
        if(!cgs.empty()){
            cgs[randomGen.random(0u,cgs.size()-1)]->setEnabled(true);
        }
    }
    void Genome::mutateActivationType() noexcept{
        if(!nodeChromosomes.empty()){
            auto selectedNode = randomGen.random(0u,nodeChromosomes.size()-1);
            nodeChromosomes[selectedNode].setActType(getRandomActivationType());
        }
    }
    void Genome::mutate(float nodeRate, float addConnRate, float removeConnRate, float perturbWeightsRate, float enableRate, float disableRate, float actTypeRate) noexcept{
        if(randomGen.random(nodeRate)){
            mutateAddNode();
        }else if(randomGen.random(addConnRate)){
            mutateAddConnection();
        }else if(randomGen.random(removeConnRate)){
            mutateRemoveConnection();
        }else if(randomGen.random(perturbWeightsRate)){
            mutateWeights(2);
        }else if(randomGen.random(enableRate)){
            mutateEnable();
        }else if(randomGen.random(disableRate)){
            mutateDisable();
        }else if(randomGen.random(actTypeRate) && cppn){
            mutateActivationType();
        }
    }
    bool Genome::isValid() noexcept{
        for(auto& n:nodeChromosomes){
            if((n.getLayerID() > 2) || (n.getNeuronID() > getNumOfNodes(n.getLayerID()))){
                return false;
            }
        }
        for(auto& c:connectionChromosomes){
            auto& src = c.getSrc();
            auto& dest = c.getDest();
            if((src.layer > 2) || (src.neuron > getNumOfNodes(src.layer)) ||
                (dest.layer > 2) || (dest.neuron > getNumOfNodes(dest.layer))){
                return false;
            }
        }
        return true;
    }
    void Genome::operator=(const Genome& rhs) noexcept{
        genomeID = rhs.genomeID;
        speciesID = rhs.speciesID;
        fitness = rhs.fitness;
        rnnAllowed = rhs.rnnAllowed;
        cppn = rhs.cppn;
        nodeChromosomes = rhs.nodeChromosomes;
        connectionChromosomes = rhs.connectionChromosomes;
    }
    void Genome::operator=(Genome&& rhs) noexcept{
        genomeID = rhs.genomeID;
        speciesID = rhs.speciesID;
        fitness = rhs.fitness;
        rnnAllowed = rhs.rnnAllowed;
        cppn = rhs.cppn;
        nodeChromosomes = std::move(rhs.nodeChromosomes);
        connectionChromosomes = std::move(rhs.connectionChromosomes);
    }
//////////
//// Static Functions
//////////
    double Genome::distance(const Genome& g1, const Genome& g2, const double& c1, const double& c2, const double& c3) noexcept{
        auto mChromo = getMatchingChromosomes(g1, g2);
        auto disGen = getDisjointGenes(g1,g2, &mChromo);
        const auto D = disGen.first.first.size() + disGen.first.second.size() + disGen.second.first.size() + disGen.second.second.size();
        auto exGen = getExcessGenes(g1,g2, &disGen);
        const auto E = exGen.first.first.size() + exGen.first.second.size() + exGen.second.first.size() + exGen.second.second.size();

        auto& cGenes1 = mChromo.second.first;
        auto& cGenes2 = mChromo.second.second;
        const auto N = std::max(g1.nodeChromosomes.size() + g1.connectionChromosomes.size(),
                                g2.nodeChromosomes.size() + g2.connectionChromosomes.size());

        auto weightAbsDiff = 0.0d;

        assert(cGenes1.size() == cGenes2.size());

        for(auto i=0u;i<cGenes1.size();++i){
            weightAbsDiff += std::abs(cGenes1[i].getWeight() - cGenes2[i].getWeight());
        }
        return ((c1 * E) / N) + ((c2 * D) / N) + c3 * weightAbsDiff;
    }
    Neuron::ActivationType Genome::getRandomActivationType() noexcept{
        return static_cast<Neuron::ActivationType>(randomGen.random(0, Neuron::ActivationType::LAST_CPPN_ACTIVATION_TYPE-1));
    }
    Genome::matchingNodeGenes Genome::getMatchingNodeGenes(const Genome& g1, const Genome& g2) noexcept{
        auto mNodeGenes = std::mismatch(std::begin(g1.nodeChromosomes), std::end(g1.nodeChromosomes)
                                                ,std::begin(g2.nodeChromosomes), std::end(g2.nodeChromosomes));
        return std::make_pair(Range<NodeGene>(std::begin(g1.nodeChromosomes), mNodeGenes.first), 
                                Range<NodeGene>(std::begin(g2.nodeChromosomes), mNodeGenes.second));
    }
    Genome::matchingConnectionGenes Genome::getMatchingConnectionGenes(const Genome& g1, const Genome& g2) noexcept{
        auto matchingConnGenes = std::mismatch(std::begin(g1.connectionChromosomes), std::end(g1.connectionChromosomes)
                                                ,std::begin(g2.connectionChromosomes), std::end(g2.connectionChromosomes));
        return std::make_pair(Range<ConnectionGene>(std::begin(g1.connectionChromosomes), matchingConnGenes.first), 
                                Range<ConnectionGene>(std::begin(g2.connectionChromosomes), matchingConnGenes.second));
    }
    Genome::matchingChromosomes Genome::getMatchingChromosomes(const Genome& g1, const Genome& g2) noexcept{
        return std::make_pair(getMatchingNodeGenes(g1, g2), getMatchingConnectionGenes(g1,g2));
    }
    Genome::excessGenes Genome::getExcessGenes(const Genome& g1, const Genome& g2, Genome::disjointGenes* hint) noexcept{
        auto disjointsGen = [&](){
            if(hint){
                return *hint;
            }else{
                return getDisjointGenes(g1, g2);
            }
        }();

        return std::make_pair(std::make_pair(Range<NodeGene>(disjointsGen.first.first.end, std::end(g1.nodeChromosomes)), 
                                                Range<NodeGene>(disjointsGen.first.second.end, std::end(g2.nodeChromosomes))), 
                                std::make_pair(Range<ConnectionGene>(disjointsGen.second.first.end, std::end(g1.connectionChromosomes)), 
                                                Range<ConnectionGene>(disjointsGen.second.second.end, std::end(g2.connectionChromosomes))));
    }
    Genome::disjointGenes Genome::getDisjointGenes(const Genome& g1, const Genome& g2, Genome::matchingChromosomes* hint) noexcept{
        const Genome* g1Ptr = &g1;
        const Genome* g2Ptr = &g2;

        auto mChromo = [&](){
            if(hint){
                return *hint;
            }else{
                return getMatchingChromosomes(g1, g2);
            }
        }();

        auto begNodes1 = mChromo.first.first.end;
        auto endNodes1 = mChromo.first.first.end;

        auto begNodes2 = mChromo.first.second.end;
        auto endNodes2 = mChromo.first.second.end;

        auto swapped = false;

        auto begConn1 = mChromo.second.first.end;
        auto endConn1 = mChromo.second.first.end;

        auto begConn2 = mChromo.second.second.end;
        auto endConn2 = mChromo.second.second.end;

        if(endNodes1 == std::end(g1.nodeChromosomes) && endNodes2 == std::end(g2.nodeChromosomes) &&
            endConn1 == std::end(g1.connectionChromosomes) && endConn2 == std::end(g2.connectionChromosomes)){
            return std::make_pair(std::make_pair(Range<NodeGene>(begNodes1, endNodes1), Range<NodeGene>(begNodes2, endNodes2)), 
                                    std::make_pair(Range<ConnectionGene>(begConn1, endConn1), Range<ConnectionGene>(begConn2, endConn2)));
        }
        auto next = [](auto iter){
            return ++iter;
        };
        if(endNodes1 != std::end(g1.nodeChromosomes) && endNodes2 != std::end(g2.nodeChromosomes)){
            if(*begNodes1 < *begNodes2){
                std::swap(begNodes1, begNodes2);
                std::swap(endNodes1, endNodes2);
                std::swap(g1Ptr, g2Ptr);
                swapped = true;
            }
            while(endNodes1 != std::end(g1Ptr->nodeChromosomes) || endNodes2 != std::end(g2Ptr->nodeChromosomes)){
                if(*endNodes2 < *endNodes1 && next(endNodes2) <= std::end(g2Ptr->nodeChromosomes)){
                    ++endNodes2;
                }else if(next(endNodes1) <= std::end(g1Ptr->nodeChromosomes)){
                    ++endNodes1;
                    break;
                }else{
                    break;
                }
            }
            if(swapped){
                std::swap(begNodes1, begNodes2);
                std::swap(endNodes1, endNodes2);
                std::swap(g1Ptr, g2Ptr);
                swapped = false;
            }
        }
        if(endConn1 != std::end(g1.connectionChromosomes) && endConn2 != std::end(g2.connectionChromosomes)){
            if(*begConn1 < *begConn2){
                std::swap(begConn1, begConn2);
                std::swap(endConn1, endConn2);
                std::swap(g1Ptr, g2Ptr);
                swapped = true;
            }
            while(endConn1 != std::end(g1Ptr->connectionChromosomes) || endConn2 != std::end(g2Ptr->connectionChromosomes)){
                if(*endConn2 < *endConn1 && next(endConn2) <= std::end(g2Ptr->connectionChromosomes)){
                    ++endConn2;
                }else if(next(endConn1) <= std::end(g1Ptr->connectionChromosomes)){
                    ++endConn1;
                    break;
                }else{
                    break;
                }
            }
            if(swapped){
                std::swap(begConn1, begConn2);
                std::swap(endConn1, endConn2);
                std::swap(g1Ptr, g2Ptr);
                swapped = false;
            }
        }
        return std::make_pair(std::make_pair(Range<NodeGene>(begNodes1, endNodes1), Range<NodeGene>(begNodes2, endNodes2)), 
                                std::make_pair(Range<ConnectionGene>(begConn1, endConn1), Range<ConnectionGene>(begConn2, endConn2)));
    }
    std::unique_ptr<Genome> Genome::reproduce(const Genome& g1, const Genome& g2) noexcept{
        if(&g1 == &g2){
            auto child = std::make_unique<Genome>(g1);
            return child;
        }
        auto child = std::make_unique<Genome>();
        std::vector<NodeGene> nGenes;
        std::vector<ConnectionGene> cGenes;
        matchingChromosomes mChromo = getMatchingChromosomes(g1,g2);

        auto dGenes = getDisjointGenes(g1,g2, &mChromo);
        auto eGenes = getExcessGenes(g1,g2, &dGenes);
        auto matchingNodeSize = mChromo.first.first.size();
        nGenes.reserve(matchingNodeSize);
        std::transform(mChromo.first.first.begin, mChromo.first.first.end, 
                        mChromo.first.second.begin, std::back_inserter(nGenes),
                            [](const auto& ng1, const auto& ng2){
                                auto selectFromFirstParent = randomGen.random(0.5);
                                if(selectFromFirstParent){
                                    return ng1;
                                }
                                return ng2;
                            }
        );
        auto matchingConnectionSize = mChromo.second.first.size();
        cGenes.reserve(matchingConnectionSize);
        for(auto i=0u;i<matchingConnectionSize;++i){
            auto selectFromFirstParent = randomGen.random(0.5);
            auto isDisabled = randomGen.random(0.5);
            if(selectFromFirstParent){
                cGenes.emplace_back(mChromo.second.first[i]);
                if(!mChromo.second.first[i].isEnabled()){
                    cGenes.back().setEnabled(isDisabled);
                }
            }else{
                cGenes.emplace_back(mChromo.second.second[i]);
                if(!mChromo.second.second[i].isEnabled()){
                    cGenes.back().setEnabled(isDisabled);
                }
            }
        }
        if(g1.getFitness() > g2.getFitness()){
            // copy g1 disjointGenes(nodeGenes and connectionGenes)
            std::copy(dGenes.first.first.begin, dGenes.first.first.end, std::back_inserter(nGenes));
            std::copy(dGenes.second.first.begin, dGenes.second.first.end, std::back_inserter(cGenes));
            // copy g1 excessGenes(nodeGenes and connectionGenes)
            std::copy(eGenes.first.first.begin, eGenes.first.first.end, std::back_inserter(nGenes));
            std::copy(eGenes.second.first.begin, eGenes.second.first.end, std::back_inserter(cGenes));
        }else if(g2.getFitness() > g1.getFitness()){
            // copy g2 disjointGenes(nodeGenes and connectionGenes)
            std::copy(dGenes.first.second.begin, dGenes.first.second.end, std::back_inserter(nGenes));
            std::copy(dGenes.second.second.begin, dGenes.second.second.end, std::back_inserter(cGenes));
            // copy g2 excessGenes(nodeGenes and connectionGenes)
            std::copy(eGenes.first.second.begin, eGenes.first.second.end, std::back_inserter(nGenes));
            std::copy(eGenes.second.second.begin, eGenes.second.second.end, std::back_inserter(cGenes));
        }else{
            // copy all disjointGenes(nodeGenes and connectionGenes)
            std::copy(dGenes.first.first.begin, dGenes.first.first.end, std::back_inserter(nGenes));
            std::copy(dGenes.first.second.begin, dGenes.first.second.end, std::back_inserter(nGenes));

            std::copy(dGenes.second.first.begin, dGenes.second.first.end, std::back_inserter(cGenes));
            std::copy(dGenes.second.second.begin, dGenes.second.second.end, std::back_inserter(cGenes));
            // copy all excessGenes(nodeGenes and connectionGenes)
            std::copy(eGenes.first.first.begin, eGenes.first.first.end, std::back_inserter(nGenes));
            std::copy(eGenes.first.second.begin, eGenes.first.second.end, std::back_inserter(nGenes));
            
            std::copy(eGenes.second.first.begin, eGenes.second.first.end, std::back_inserter(cGenes));
            std::copy(eGenes.second.second.begin, eGenes.second.second.end, std::back_inserter(cGenes));
        }
        std::sort(std::begin(nGenes), std::end(nGenes));
        std::sort(std::begin(cGenes), std::end(cGenes));
        nGenes.erase(std::unique(std::begin(nGenes), std::end(nGenes)), std::end(nGenes));
        cGenes.erase(std::unique(std::begin(cGenes), std::end(cGenes)), std::end(cGenes));
        child->setNodeChromosomes(std::move(nGenes));
        child->setConnectionChromosomes(std::move(cGenes));
        return child;
    }
    std::unique_ptr<NeuralNetwork> Genome::makePhenotype(const Genome& g) noexcept{
        auto nn = std::make_unique<NeuralNetwork>();
        NeuronLayer inputLayer;
        inputLayer.setType(Neuron::Type::INPUT);
        inputLayer.setBias(1.0);
        NeuronLayer hiddenLayer;
        hiddenLayer.setType(Neuron::Type::HIDDEN);
        hiddenLayer.setBias(1.0);
        NeuronLayer outputLayer;
        outputLayer.setType(Neuron::Type::OUTPUT);
        outputLayer.setBias(1.0);
        for(auto& n:g.getNodeChromosomes()){
            Neuron nrn(n.getNeuronType());
            nrn.setActivationType(n.getActType());
            nrn.setBiasWeight(n.getBias());
            switch(n.getNeuronType()){
                case Neuron::Type::INPUT:
                    inputLayer.addNeuron(nrn);
                    break;
                case Neuron::Type::CONTEXT:
                case Neuron::Type::HIDDEN:
                    hiddenLayer.addNeuron(nrn);
                    break;
                case Neuron::Type::OUTPUT:
                    outputLayer.addNeuron(nrn);
                    break;
            }
        }
        nn->addLayer(inputLayer);
        nn->addLayer(hiddenLayer);
        nn->addLayer(outputLayer);
        for(auto& c:g.getConnectionChromosomes()){
            if(c.isEnabled()){
                nn->addConnection(c.getConnection());
            }
        }
        return nn;
    }
    std::unique_ptr<Genome> Genome::makeGenome(NeuralNetwork& nn) noexcept{
        auto g = std::make_unique<Genome>();
        std::vector<NodeGene> nGenes;
        std::vector<ConnectionGene> cGenes;
        // add inputs
        auto& inputs = nn[0].getNeurons();
        for(auto i=0u;i<inputs.size();++i){
            auto ng = NodeGene(0,i);
            ng.setActType(inputs[i].getActivationType());
            ng.setNeuronType(inputs[i].getType());
            nGenes.push_back(ng);
        }
        // add outputs
        auto outputNum = (nn.size()-1);
        auto& outputs = nn[outputNum].getNeurons();
        for(auto i=0u;i<outputs.size();++i){
            auto ng = NodeGene(2,i);
            ng.setActType(outputs[i].getActivationType());
            ng.setNeuronType(outputs[i].getType());
            nGenes.push_back(ng);
        }
        // add hidden
        auto lastHiddenLayer = (nn.size()-1);
        auto id = 0;
        for(auto i=1u;i<lastHiddenLayer;++i){
            auto& hidden = nn[i].getNeurons();
            for(auto j=0u;j<hidden.size();++j){
                auto ng = NodeGene(1,id++); // get id and increment
                ng.setActType(hidden[j].getActivationType());
                ng.setNeuronType(hidden[j].getType());
                nGenes.push_back(ng);
            }
        }
        // add connections
        auto outputLayer = nn.size() - 1;
        for(auto& c:nn.getConnections()){
            auto src = c->getSrc();
            auto srcLayer = src.layer;
            auto srcID = src.neuron;
            auto dest = c->getDest();
            auto destLayer = dest.layer;
            auto destID = dest.neuron;
            if(srcLayer == outputLayer){
                srcLayer = 2;
            }else if(srcLayer > 1){
                for(auto s=1u;s<srcLayer;++s){
                    srcID += (nn[s].size());
                }
                srcLayer = 1;
            }
            if(destLayer == outputLayer){
                destLayer = 2;
            }else if(destLayer > 1){
                for(auto s=1u;s<destLayer;++s){
                    destID += (nn[s].size());
                }
                destLayer = 1;
            }
            auto cg = ConnectionGene(Link(srcLayer,srcID), Link(destLayer,destID), c->getWeight());
            cGenes.push_back(cg);
        }
        g->setNodeChromosomes(std::move(nGenes));
        g->setConnectionChromosomes(std::move(cGenes));
        return g;
    }
}