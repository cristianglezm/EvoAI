#include <EvoAI/Genome.hpp>

#include <random>
#include <EvoAi/NeuronLayer.hpp>

namespace EvoAI{
    Genome::Genome()
    : genomeID(0)
    , speciesID(0)
    , fitness(0.0)
    , rnnAllowed(true)
    , cppn(false)
    , nodeChromosomes()
    , connectionChromosomes(){}
    Genome::Genome(std::size_t numInputs, std::size_t numOutputs, bool canBeRecursive, bool cppn)
    : genomeID(0)
    , speciesID(0)
    , fitness(0.0)
    , rnnAllowed(canBeRecursive)
    , cppn(cppn)
    , nodeChromosomes()
    , connectionChromosomes(){
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
        for(auto i=0u;i<numInputs;++i){
            for(auto j=0u;j<numOutputs;++j){
                connectionChromosomes.emplace_back(NodeGene(0,i), NodeGene(2,j), random(-3.0,3.0));
            }
        }
    }
    Genome::Genome(std::size_t numInputs, std::size_t numHidden, std::size_t numOutputs, bool canBeRecursive, bool cppn)
    : genomeID(0)
    , speciesID(0)
    , fitness(0.0)
    , rnnAllowed(canBeRecursive)
    , cppn(cppn)
    , nodeChromosomes()
    , connectionChromosomes(){
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
        for(auto i=0u;i<numInputs;++i){
            for(auto j=0u;j<numHidden;++j){
                connectionChromosomes.emplace_back(NodeGene(0,i), NodeGene(1,j), random(-3.0,3.0));
            }
        }
        for(auto i=0u;i<numHidden;++i){
            for(auto j=0u;j<numOutputs;++j){
                connectionChromosomes.emplace_back(NodeGene(1,i), NodeGene(2,j), random(-3.0,3.0));
            }
        }
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
        for(auto& ng:ngs){
            nodeChromosomes.emplace_back(ng.getObject());
        }
        auto& cgs = o["ConnectionChromosomes"].getArray();
        for(auto& cg:cgs){
            connectionChromosomes.emplace_back(cg.getObject());
        }
    }
    Genome::Genome(const std::string& jsonfile)
    : genomeID(0)
    , speciesID(0)
    , fitness(0.0)
    , rnnAllowed(true)
    , cppn(true)
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
        for(auto& ng:ngs){
            nodeChromosomes.emplace_back(ng.getObject());
        }
        auto& cgs = v["ConnectionChromosomes"].getArray();
        for(auto& cg:cgs){
            connectionChromosomes.emplace_back(cg.getObject());
        }
    }
    void Genome::addGene(const NodeGene& ng) noexcept{
        nodeChromosomes.push_back(ng);
    }
    void Genome::addGene(const ConnectionGene& cg) noexcept{
        connectionChromosomes.push_back(cg);
    }
    void Genome::setNodeChromosomes(std::vector<NodeGene>&& ngenes) noexcept{
        nodeChromosomes = std::move(ngenes);
    }
    std::vector<NodeGene>& Genome::getNodeChromosomes() noexcept{
        return nodeChromosomes;
    }
    const std::vector<NodeGene>& Genome::getNodeChromosomes() const noexcept{
        return nodeChromosomes;
    }
    void Genome::setConnectionChromosomes(std::vector<ConnectionGene>&& cgenes) noexcept{
        connectionChromosomes = std::move(cgenes);
    }
    std::vector<ConnectionGene>& Genome::getConnectionChromosomes() noexcept{
        return connectionChromosomes;
    }
    const std::vector<ConnectionGene>& Genome::getConnectionChromosomes() const noexcept{
        return connectionChromosomes;
    }
    std::size_t Genome::getNumOfNodes(std::size_t layerID) const noexcept{
        std::size_t num = 0;
        for(auto& n:nodeChromosomes){
            if(n.getLayerID() == layerID){
                ++num;
            }
        }
        return num;
    }
    JsonBox::Value Genome::toJson() noexcept{
        JsonBox::Object o;
        o["GenomeID"] = JsonBox::Value(std::to_string(genomeID));
        o["SpeciesID"] = JsonBox::Value(std::to_string(speciesID));
        o["fitness"] = JsonBox::Value(fitness);
        o["cppn"] = JsonBox::Value(cppn);
        o["rnnAllowed"] = JsonBox::Value(rnnAllowed);
        JsonBox::Array nChromo;
        for(auto& n:nodeChromosomes){
            nChromo.push_back(n.toJson());
        }
        JsonBox::Array cChromo;
        for(auto& c:connectionChromosomes){
            cChromo.push_back(c.toJson());
        }
        o["nodeChromosomes"] = JsonBox::Value(nChromo);
        o["ConnectionChromosomes"] = JsonBox::Value(cChromo);
        return JsonBox::Value(o);
    }
    void Genome::writeToFile(const std::string& filename) noexcept{
        JsonBox::Value v;
        v["version"] = JsonBox::Value("1.0");
        v["Genome"] = toJson();
        v.writeToFile(filename);
    }
    void Genome::setFitness(const double& fit) noexcept{
        fitness = fit;
    }
    const double& Genome::getFitness() const noexcept{
        return fitness;
    }
    bool Genome::hasNodeGene(const NodeGene& ng) const noexcept{
        auto found = std::find(std::begin(nodeChromosomes),std::end(nodeChromosomes),ng);
        return (found != std::end(nodeChromosomes));
    }
    bool Genome::hasConnectionGene(const ConnectionGene& cg) const noexcept{
        auto found = std::find(std::begin(connectionChromosomes),std::end(connectionChromosomes),cg);
        return (found != std::end(connectionChromosomes));
    }
    void Genome::setGenomeID(std::size_t gnmID) noexcept{
        genomeID = gnmID;
    }
    const std::size_t& Genome::getGenomeID() const noexcept{
        return genomeID;
    }
    void Genome::setSpeciesID(std::size_t spcID) noexcept{
        speciesID = spcID;
    }
    const std::size_t& Genome::getSpeciesID() const noexcept{
        return speciesID;
    }
    void Genome::mutateAddNode() noexcept{
        if(!connectionChromosomes.empty()){
            auto selectedConnection = random(0,connectionChromosomes.size()-1);
            auto& selConn = connectionChromosomes[selectedConnection];
            auto at = Neuron::ActivationType::SIGMOID;
            if(cppn){
                at = getRandomActivationType();
            }
            NodeGene ng(1,getNumOfNodes(1),Neuron::Type::HIDDEN,at);
            nodeChromosomes.push_back(ng);
            selConn.setEnabled(false);
            ConnectionGene cg1(NodeGene(selConn.getSrc().layer,selConn.getSrc().neuron), ng, 1.0);
            ConnectionGene cg2(ng,NodeGene(selConn.getDest().layer,selConn.getDest().neuron), selConn.getWeight());
            connectionChromosomes.push_back(cg1);
            connectionChromosomes.push_back(cg2);
        }
    }
    void Genome::mutateAddConnection() noexcept{
        if(!nodeChromosomes.empty()){
            auto selectedNode1 = random(0,nodeChromosomes.size()-1);
            auto selectedNode2 = random(0,nodeChromosomes.size()-1);
            if(!rnnAllowed){
                if(nodeChromosomes[selectedNode1].getLayerID() < nodeChromosomes[selectedNode2].getLayerID()){
                    connectionChromosomes.emplace_back(nodeChromosomes[selectedNode1], nodeChromosomes[selectedNode2], random(-3.0,3.0));
                }else{
                    connectionChromosomes.emplace_back(nodeChromosomes[selectedNode2], nodeChromosomes[selectedNode1], random(-3.0,3.0));
                }
            }else{
                connectionChromosomes.emplace_back(nodeChromosomes[selectedNode1], nodeChromosomes[selectedNode2], random(-3.0,3.0));
            }
        }
    }
    void Genome::mutateRemoveConnection() noexcept{
        if(!connectionChromosomes.empty()){
            auto selectedConn = random(0,connectionChromosomes.size()-1);
            connectionChromosomes.erase(std::remove(std::begin(connectionChromosomes),
                                                std::end(connectionChromosomes),
                                                connectionChromosomes[selectedConn]),
                                                std::end(connectionChromosomes));
        }
    }
    void Genome::mutateWeights(double power) noexcept{
        auto nodeOrConn = doAction(0.5);
        auto shakeThingsUp = doAction(0.5);
        auto isNegative = doAction(0.5);
        if(nodeOrConn){
            if(nodeChromosomes.empty()){
                return;
            }
            auto selectedNode = random(0,nodeChromosomes.size() - 1);
            auto isOld = ((static_cast<std::size_t>(selectedNode)) < (nodeChromosomes.size() / 2));
            if(isOld){
                power += (power * power) * 0.8;
            }
            auto weight = power * random(0.0,2.0);
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
            auto selectedConnection = random(0,connectionChromosomes.size() - 1);
            if(connectionChromosomes[selectedConnection].isFrozen()){
                return;
            }
            auto isOld = ((static_cast<std::size_t>(selectedConnection)) < (connectionChromosomes.size() / 2));
            if(isOld){
                power += (power*power) * 0.8;
            }
            auto weight = power * random(0.0,2.0);
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
        for(auto& c:connectionChromosomes){
            if(c.isEnabled()){
                cgs.push_back(&c);
            }
        }
        if(!cgs.empty()){
            cgs[random(0,cgs.size()-1)]->setEnabled(false);
        }
    }
    void Genome::mutateEnable() noexcept{
        std::vector<ConnectionGene*> cgs;
        for(auto& c:connectionChromosomes){
            if(!c.isEnabled()){
                cgs.push_back(&c);
            }
        }
        if(!cgs.empty()){
            cgs[random(0,cgs.size()-1)]->setEnabled(true);
        }
    }
    void Genome::mutateActivationType() noexcept{
        if(!nodeChromosomes.empty()){
            auto selectedNode = random(0,nodeChromosomes.size()-1);
            nodeChromosomes[selectedNode].setActType(getRandomActivationType());
        }
    }
    void Genome::mutate(float nodeRate, float addConnRate, float removeConnRate, float perturbWeightsRate, float enableRate, float disableRate, float actTypeRate) noexcept{
        if(doAction(nodeRate)){
            mutateAddNode();
        }else if(doAction(addConnRate)){
            mutateAddConnection();
        }else if(doAction(removeConnRate)){
            mutateRemoveConnection();
        }else if(doAction(perturbWeightsRate)){
            mutateWeights(2);
        }else if(doAction(enableRate)){
            mutateEnable();
        }else if(doAction(disableRate)){
            mutateDisable();
        }else if(doAction(actTypeRate)){
            if(cppn){
                mutateActivationType();
            }
        }
    }
    bool Genome::isValid() noexcept{
        for(auto& n:nodeChromosomes){
            if(((n.getLayerID() < 0) || (n.getLayerID() > 3)) 
                    || (n.getNeuronID() >= getNumOfNodes(n.getLayerID()))){
                return false;
            }
        }
        for(auto& c:connectionChromosomes){
            auto& src = c.getSrc();
            auto& dest = c.getDest();
            if((src.layer > 3) || (src.layer < 0)
                || (src.neuron >= getNumOfNodes(src.layer))
                || (dest.layer > 3) || (dest.layer < 0)
                || (dest.neuron >= getNumOfNodes(dest.layer))){
                return false;
            }
        }
        return true;
    }
//////////
//// Static Functions
//////////
    double Genome::distance(const Genome& g1, const Genome& g2, const double& c1, const double& c2, const double& c3) noexcept{
        auto E = getExcessGenes(g1,g2).second.size();
        auto D = getDisjointGenes(g1,g2).second.size();
        auto matchingGenes = getMatchingChromosomes(g1, g2);
        auto N = matchingGenes.second.first.size();
        auto weightAvgs = 0.0;
        for(auto& c1:matchingGenes.second.first){
            for(auto& c2:matchingGenes.second.second){
                weightAvgs += c1.getWeight() - c2.getWeight();
            }
        }
        weightAvgs /= matchingGenes.second.first.size();
        return ((c1*E)/N) + ((c2*D)/N) + c3 * weightAvgs;
    }
    Neuron::ActivationType Genome::getRandomActivationType() noexcept{
        return static_cast<Neuron::ActivationType>(random(0, Neuron::ActivationType::LAST_CPPN_ACTIVATION_TYPE-1));
    }
    Genome::matchingNodeGenes Genome::getMatchingNodeGenes(const Genome& g1, const Genome& g2) noexcept{
        std::vector<NodeGene> matchingNodes1;
        std::vector<NodeGene> matchingNodes2;
        for(auto& n1:g1.nodeChromosomes){
            for(auto& n2:g2.nodeChromosomes){
                if(n1 == n2){
                    matchingNodes1.push_back(n1);
                    matchingNodes2.push_back(n2);
                }
            }
        }
        return std::make_pair(matchingNodes1, matchingNodes2);
    }
    Genome::matchingConnectionGenes Genome::getMatchingConnectionGenes(const Genome& g1, const Genome& g2) noexcept{
        std::vector<ConnectionGene> matchingConnections1;
        std::vector<ConnectionGene> matchingConnections2;
        for(auto& c1:g1.connectionChromosomes){
            for(auto& c2:g2.connectionChromosomes){
                if(c1 == c2){
                    matchingConnections1.push_back(c1);
                    matchingConnections2.push_back(c2);
                }
            }
        }
        return std::make_pair(matchingConnections1, matchingConnections2);
    }
    Genome::matchingChromosomes Genome::getMatchingChromosomes(const Genome& g1, const Genome& g2) noexcept{
        return std::make_pair(getMatchingNodeGenes(g1, g2), getMatchingConnectionGenes(g1, g2));
    }
    Genome::excessGenes Genome::getExcessGenes(const Genome& g1, const Genome& g2) noexcept{
        excessNodeGenes eng;
        excessConnectionGenes ecg;
        auto end1 = g2.nodeChromosomes.end() - 1;
        auto end2 = g2.connectionChromosomes.end() - 1;
        for(auto& n1:g1.nodeChromosomes){
            if(!g2.hasNodeGene(n1) 
                && n1.getInnovationID() > (*end1).getInnovationID()){
                eng.push_back(n1);
            }
        }
        if(!g2.connectionChromosomes.empty()){
            for(auto& c1:g1.connectionChromosomes){
                if(!g2.hasConnectionGene(c1)
                    && c1.getInnovationID() > (*end2).getInnovationID()){
                    ecg.push_back(c1);
                }
            }
        }else{
            for(auto& c1:g1.connectionChromosomes){
                ecg.push_back(c1);
            }
        }
        return std::make_pair(eng,ecg);
    }
    Genome::disjointGenes Genome::getDisjointGenes(const Genome& g1, const Genome& g2) noexcept{
        disjointNodeGenes dng;
        disjointConnectionGenes dcg;
        auto end1 = g2.nodeChromosomes.end() - 1;
        auto end2 = g2.connectionChromosomes.end() - 1;
        for(auto& n1:g1.nodeChromosomes){
            if(!g2.hasNodeGene(n1) 
                && n1.getInnovationID() < (*end1).getInnovationID()){
                dng.push_back(n1);
            }
        }
        if(!g2.connectionChromosomes.empty()){
            for(auto& c1:g1.connectionChromosomes){
                if(!g2.hasConnectionGene(c1)
                    && c1.getInnovationID() < (*end2).getInnovationID()){
                    dcg.push_back(c1);
                }
            }
        }else{
            for(auto& c1:g1.connectionChromosomes){
                dcg.push_back(c1);
            }
        }
        return std::make_pair(dng,dcg);
    }
    std::unique_ptr<Genome> Genome::reproduce(const Genome& g1, const Genome& g2) noexcept{
        auto child = std::make_unique<Genome>();
        std::vector<NodeGene> nGenes;
        std::vector<ConnectionGene> cGenes;
        if(&g1 == &g2){
            for(auto& ng:g1.getNodeChromosomes()){
                nGenes.push_back(ng);
            }
            for(auto& cg:g1.getConnectionChromosomes()){
                cGenes.push_back(cg);
            }
            nGenes.erase(std::unique(std::begin(nGenes), std::end(nGenes),
                                        [](NodeGene& ng1,NodeGene& ng2){
                                            return (ng1.getInnovationID() == ng2.getInnovationID());
                                        }), std::end(nGenes));
            cGenes.erase(std::unique(std::begin(cGenes), std::end(cGenes),
                                    [](ConnectionGene& cg1,ConnectionGene& cg2){
                                        return (cg1.getInnovationID() == cg2.getInnovationID());
                                    }), std::end(cGenes));
            child->setNodeChromosomes(std::move(nGenes));
            child->setConnectionChromosomes(std::move(cGenes));
            return child;
        }
        matchingChromosomes mChromo = getMatchingChromosomes(g1,g2);
        auto matchingNodeSize = mChromo.first.first.size();
        for(auto i=0u;i<matchingNodeSize;++i){
            auto selectFromFirstParent = doAction(0.5);
            if(selectFromFirstParent){
                nGenes.push_back(mChromo.first.first[i]);
            }else{
                nGenes.push_back(mChromo.first.second[i]);
            }
        }
        auto matchingConnectionSize = mChromo.second.first.size();
        for(auto i=0u;i<matchingConnectionSize;++i){
            auto selectFromFirstParent = doAction(0.5);
            auto isDisabled = doAction(0.5);
            if(selectFromFirstParent){
                if(!mChromo.second.first[i].isEnabled()
                    || !mChromo.second.second[i].isEnabled()){
                        mChromo.second.first[i].setEnabled(isDisabled);
                }
                cGenes.push_back(mChromo.second.first[i]);
            }else{
                if(!mChromo.second.first[i].isEnabled()
                    || !mChromo.second.second[i].isEnabled()){
                        mChromo.second.second[i].setEnabled(isDisabled);
                }
                cGenes.push_back(mChromo.second.second[i]);
            }
        }
        if(g1.getFitness() > g2.getFitness()){
            auto eGenes = getExcessGenes(g1,g2);
            for(auto& g:eGenes.first){
                nGenes.push_back(g);
            }
            for(auto& g:eGenes.second){
                cGenes.push_back(g);
            }
            auto dGenes = getDisjointGenes(g1,g2);
            for(auto& g:dGenes.first){
                nGenes.push_back(g);
            }
            for(auto& g:dGenes.second){
                cGenes.push_back(g);
            }
        }else if(g2.getFitness() > g1.getFitness()){
            auto eGenes = getExcessGenes(g2,g1);
            for(auto& g:eGenes.first){
                nGenes.push_back(g);
            }
            for(auto& g:eGenes.second){
                cGenes.push_back(g);
            }
            auto dGenes = getDisjointGenes(g2,g1);
            for(auto& g:dGenes.first){
                nGenes.push_back(g);
            }
            for(auto& g:dGenes.second){
                cGenes.push_back(g);
            }
        }else{
            for(auto& n1:g1.nodeChromosomes){
                if(!g2.hasNodeGene(n1)){
                    nGenes.push_back(n1);
                }
            }
            for(auto& c1:g1.connectionChromosomes){
                if(!g2.hasConnectionGene(c1)){
                    cGenes.push_back(c1);
                }
            }
            for(auto& n2:g2.nodeChromosomes){
                if(!g1.hasNodeGene(n2)){
                    nGenes.push_back(n2);
                }
            }
            for(auto& c2:g2.connectionChromosomes){
                if(!g1.hasConnectionGene(c2)){
                    cGenes.push_back(c2);
                }
            }
        }
        nGenes.erase(std::unique(std::begin(nGenes), std::end(nGenes),
                                        [](NodeGene& ng1,NodeGene& ng2){
                                            return (ng1.getInnovationID() == ng2.getInnovationID());
                                        }), std::end(nGenes));
        cGenes.erase(std::unique(std::begin(cGenes), std::end(cGenes),
                                    [](ConnectionGene& cg1,ConnectionGene& cg2){
                                        return (cg1.getInnovationID() == cg2.getInnovationID());
                                    }), std::end(cGenes));
        child->setNodeChromosomes(std::move(nGenes));
        child->setConnectionChromosomes(std::move(cGenes));
        return child;
    }
    std::unique_ptr<NeuralNetwork> Genome::makePhenotype(Genome& g) noexcept{
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
}