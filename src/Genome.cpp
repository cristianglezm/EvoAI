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
            // activation function is not used by Neuron::Type::INPUT
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
                connectionChromosomes.emplace_back(NodeGene(0,i), NodeGene(2,j), random(-5.0,5.0));
            }
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
        genomeID = std::stoul(v["GenomeID"].getString());
        speciesID = std::stoul(v["SpeciesID"].getString());
        fitness = v["fitness"].getFloat();
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
    void Genome::setConnectionChromosomes(std::vector<ConnectionGene>&& cgenes) noexcept{
        connectionChromosomes = std::move(cgenes);
    }
    std::vector<ConnectionGene>& Genome::getConnectionChromosomes() noexcept{
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
    void Genome::mutateAddConnection() noexcept{
        auto selectedNode1 = random(0,nodeChromosomes.size()-1);
        auto selectedNode2 = random(0,nodeChromosomes.size()-1);
        ConnectionGene cg(nodeChromosomes[selectedNode1],nodeChromosomes[selectedNode2],random(-5.0,5.0));
        connectionChromosomes.push_back(cg);
    }
    void Genome::mutate() noexcept{
        /// @todo mutate args rates etc
        float nodeRate = 0.3, connectionRate = 0.4, perturbWeightsRate = 0.7; /// @todo remove and add args
        if(doAction(nodeRate)){
            mutateAddNode();
        }
        if(doAction(connectionRate)){
            mutateAddConnection();
        }
        if(doAction(perturbWeightsRate)){
            /// @todo mutate weights
            mutateWeights(2);
        }
    }
    /// @todo look at src genome NEAT
    void Genome::mutateWeights(double power) noexcept{
        /// @todo implement also mutate node Weights
    }
    void Genome::mutateEnable() noexcept{
        for(auto& c:connectionChromosomes){
            if(!c.isEnabled()){
                c.setEnabled(true);
                break;
            }
        }
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
        /// @todo fix only get excess genes
        excessNodeGenes eng;
        excessConnectionGenes ecg;
        for(auto& n1:g1.nodeChromosomes){
            if(!g2.hasNodeGene(n1)){
                eng.push_back(n1);
            }
        }
        for(auto& c1:g1.connectionChromosomes){
            if(!g2.hasConnectionGene(c1)){
                ecg.push_back(c1);
            }
        }
        return std::make_pair(eng,ecg);
    }
    Genome::disjointGenes Genome::getDisjointGenes(const Genome& g1, const Genome& g2) noexcept{
        /// @todo fix only get disjoint nodes.
        disjointNodeGenes dng;
        disjointConnectionGenes dcg;
        return std::make_pair(dng,dcg);
    }
    std::unique_ptr<Genome> Genome::reproduce(const Genome& g1, const Genome& g2) noexcept{
        auto child = std::make_unique<Genome>();
        std::vector<NodeGene> nGenes;
        std::vector<ConnectionGene> cGenes;
        matchingChromosomes mChromo = getMatchingChromosomes(g1,g2);
        // assume g1 and g2 matchingChromosomes are the same size.
        for(auto i=0u;i<mChromo.first.first.size();++i){
            auto selectFromFirstParent = doAction(0.5);
            if(selectFromFirstParent){
                nGenes.push_back(mChromo.first.first[i]);
            }else{
                nGenes.push_back(mChromo.first.second[i]);
            }
            selectFromFirstParent = doAction(0.5);
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
        /// @todo select excess and disjoint add them to n/cGenes
        if(g1.getFitness() > g2.getFitness()){
            ///@todo select excess and disjoint from g1
            auto genes = getExcessGenes(g1,g2);
            for(auto& g:genes.first){
                nGenes.push_back(g);
            }
            for(auto& g:genes.second){
                cGenes.push_back(g);
            }
        }else{
            ///@todo select excess and disjoint from g2
            auto genes = getExcessGenes(g2,g1);
            for(auto& g:genes.first){
                nGenes.push_back(g);
            }
            for(auto& g:genes.second){
                cGenes.push_back(g);
            }
        }
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