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
        for(auto i=0u;i<numInputs;++i){
            // activation function is not used by Neuron::Type::INPUT
            nodeChromosomes.emplace_back(0,i,Neuron::Type::INPUT,Neuron::ActivationType::SIGMOID);
        }
        for(auto i=0u;i<numOutputs;++i){
            Neuron::ActivationType at = Neuron::ActivationType::SIGMOID;
            if(cppn){
                unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
                std::mt19937 g(seed);
                std::uniform_int_distribution<int> dice(0,Neuron::ActivationType::LAST_CPPN_ACTIVATION_TYPE-1);
                at = static_cast<Neuron::ActivationType>(dice(g));
            }
            nodeChromosomes.emplace_back(2,i,Neuron::Type::OUTPUT,at);
        }
        /// @todo make feedforwards connections for the simple structure.
    }
    /// @todo node is added, where an old connection was,
    ///       then the src to new node weight is 1 and the
    ///       new node to dest is equal to the old weight.
    /// *--*
    /// *-w = 1-*(new node)-old w-*
    void Genome::addNodeGene(const NodeGene& ng) noexcept{
        nodeChromosomes.push_back(ng);
    }
    void Genome::addConnectionGene(const ConnectionGene& cg) noexcept{
        connectionChromosomes.push_back(cg);
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
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        static std::mt19937 rEngine(seed);
        float mutationRate = 0.6, nodeRate = 0.3, connectionRate = 0.4; /// @todo remove add args
        std::bernoulli_distribution isMutating(mutationRate);
        std::bernoulli_distribution addNode(nodeRate);
        std::bernoulli_distribution addConnection(connectionRate);
        if(isMutating(rEngine)){
            if(addNode(rEngine)){
                /// @todo select a connection and slice it add a node and 2 connections connecting the
                ///         old node to the new node and the new node to another node from the old connection
                
            }else if(addConnection(rEngine)){
                /// @ todo add a new connection
            }
        }
    }
    /// look at src genome NEAT
    void Genome::mutateWeights(double power, double rate) noexcept{
        /// @todo
    }
    void Genome::mutateEnable() noexcept{
        /// @todo
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
    std::unique_ptr<NeuralNetwork> Genome::makePhenotype() noexcept{
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