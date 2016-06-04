#include <EvoAI/Genome.hpp>

#include <random>

namespace EvoAI{
    Genome::Genome(std::size_t numInputs, std::size_t numOutputs, bool canBeRecursive, bool cppn)
    : genomeID(0)
    , speciesID(0)
    , fitness(0.0)
    , rnnPermited(canBeRecursive)
    , nodeChromosomes()
    , connectionChromosomes(){
        /// @todo make feedforwards connections for the simple structure.
        for(auto i=0u;i<numInputs;++i){
            // activation function is not used by Neuron::Type::INPUT
            nodeChromosomes.emplace_back(0,i,Neuron::Type::INPUT,Neuron::ActivationType::SIGMOID);
            auto innv = getLastInnovationNode() + 1;
            nodeChromosomes[i].setInnovationID(innv);
        }
        for(auto i=0u;i<numOutputs;++i){
            Neuron::ActivationType at = Neuron::ActivationType::SIGMOID;
            if(cppn){
                at = Genome::getRandomActivationType();
            }
            nodeChromosomes.emplace_back(2,i,Neuron::Type::OUTPUT,at);
            auto innv = getLastInnovationNode() + 1;
            nodeChromosomes[i].setInnovationID(innv);
        }
        for(auto i=0u;i<numInputs;++i){
            for(auto j=0u;j<numOutputs;++j){
                connectionChromosomes.emplace_back(NodeGene(0,i), NodeGene(2,j), random(-5.0,5.0));
            }
        }
    }
    /// @todo node is added, where an old connection was,
    ///       then the src to new node weight is 1 and the
    ///       new node to dest is equal to the old weight.
    /// *--*
    /// *-w = 1-*(new node)-old w-*
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
    JsonBox::Value Genome::toJson() noexcept{
        /// @todo
    }
    std::size_t Genome::getLastInnovationNode() const noexcept{
        if(nodeChromosomes.empty()){
            return 0;
        }
        return nodeChromosomes.back().getInnovationID();
    }
    std::size_t Genome::getLastInnovationConnection() const noexcept{
        if(connectionChromosomes.empty()){
            return 0;
        }
        return connectionChromosomes.back().getInnovationID();
    }
    void Genome::mutate() noexcept{
        /// @todo
        float nodeRate = 0.3, connectionRate = 0.4, perturbWeightsRate = 0.7; /// @todo remove and add args
        if(doAction(nodeRate)){
            /// @todo select a connection and slice it add a node and 2 connections connecting the
            ///         old node to the new node and the new node to another node from the old connection
            
        }else if(doAction(connectionRate)){
            /// @todo add a new connection
        }else if(doAction(perturbWeightsRate)){
            
        }
    }
    /// look at src genome NEAT
    void Genome::mutateWeights(double power, double rate) noexcept{
        /// @todo
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
        auto E = excessGenes(g1,g2);
        auto D = disjointGenes(g1,g2);
        auto matchingGenes = Genome::getMatchingChromosomes(g1, g2);
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
    Genome::matchingChromosomes Genome::getMatchingChromosomes(const Genome& g1, const Genome& g2) noexcept{
        std::vector<NodeGene> matchingNodes1;
        std::vector<NodeGene> matchingNodes2;
        std::vector<ConnectionGene> matchingConnections1;
        std::vector<ConnectionGene> matchingConnections2;
        for(auto& n1:g1.nodeChromosomes){
            for(auto& n2:g2.nodeChromosomes){
                if(n1 == n2){
                    matchingNodes1.push_back(n1);
                    matchingNodes2.push_back(n2);
                }
            }
        }
        for(auto& c1:g1.connectionChromosomes){
            for(auto& c2:g2.connectionChromosomes){
                if(c1 == c2){
                    matchingConnections1.push_back(c1);
                    matchingConnections2.push_back(c2);
                }
            }
        }
        return std::make_pair(std::make_pair(matchingNodes1,matchingNodes2), 
                                std::make_pair(matchingConnections1,matchingConnections2));
    }
    /// genes are randomly chosen from either parent at matching genes,
    /// whereas all excess or disjoint genes are always included from the more fit parent
    /// there’s a preset chance that an inherited gene is disabled if it is disabled in either parent
    std::unique_ptr<Genome> Genome::reproduce(const Genome& g1, const Genome& g2) noexcept{
        auto child = std::make_unique<Genome>(); /// @todo
        return child;
    }
    std::unique_ptr<NeuralNetwork> Genome::makePhenotype(const Genome& g) noexcept{
        auto nn = std::make_unique<NeuralNetwork>(); /// @todo
        return nn;
    }
    double Genome::excessGenes(const Genome& g1, const Genome& g2) noexcept{
        return 0.0; /// @todo
    }
    double Genome::disjointGenes(const Genome& g1, const Genome& g2) noexcept{
        return 0.0; /// @todo
    }
}