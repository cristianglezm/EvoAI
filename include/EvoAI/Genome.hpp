#ifndef EVOAI_GENOME_HPP
#define EVOAI_GENOME_HPP

#include <JsonBox.h>
#include <EvoAI/Connection.hpp>
#include <EvoAI/Neuron.hpp>
#include <EvoAi/NeuralNetwork.hpp>
#include <memory>
#include <functional>
#include <chrono>

namespace EvoAI{
    /**
     @todo
     
      * Shared fitness with species
      * list of the innovations that occurred in the current generation
     */
    class Genome final{
        public:
            class NodeGene final{
                public:
                    NodeGene(std::size_t lyrID, std::size_t nrnID,
                                Neuron::Type nt, Neuron::ActivationType nat);
                    JsonBox::Value toJson() noexcept;
                    std::size_t getLayerID() const noexcept;
                    std::size_t getNeuronID() const noexcept;
                    Neuron::Type getNeuronType() const noexcept;
                    Neuron::ActivationType getActType() const noexcept;
                    void setInnovationID(const std::size_t& id) noexcept;
                    const std::size_t& getInnovationID() const noexcept;
                    bool operator==(const NodeGene& rhs) const{
                        return (layerID == rhs.layerID
                                && neuronID == rhs.neuronID
                                && nrnType == rhs.nrnType
                                && actType == rhs.actType
                                && innovationID == rhs.innovationID);
                    }
                    bool operator!=(const NodeGene& rhs) const{
                        return !((*this)==rhs);
                    }
                    ~NodeGene() = default;
                private:
                    std::size_t layerID;
                    std::size_t neuronID;
                    Neuron::Type nrnType;
                    Neuron::ActivationType actType;
                    std::size_t innovationID;
            };
            class ConnectionGene final{
                public:
                    ConnectionGene(const NodeGene& src, const NodeGene& dest, double w);
                    bool isEnabled() const noexcept;
                    void setEnabled(bool en) noexcept;
                    JsonBox::Value toJson() noexcept;
                    void setWeight(double w) noexcept;
                    const double& getWeight() const noexcept;
                    void setInnovationID(const std::size_t& id) noexcept;
                    const std::size_t& getInnovationID() const noexcept;
                    bool operator==(const ConnectionGene& rhs) const{
                        return (enabled == rhs.enabled
                                && c == rhs.c
                                && innovationID == rhs.innovationID);
                    }
                    bool operator!=(const ConnectionGene& rhs){
                        return !((*this)==rhs);
                    }
                    ~ConnectionGene() = default;
            private:
                    bool enabled;
                    Connection c;
                    std::size_t innovationID;
            };
            using matchingChromosomes = std::pair<std::pair<std::vector<Genome::NodeGene>, std::vector<Genome::NodeGene>>, 
                                        std::pair<std::vector<Genome::ConnectionGene>, std::vector<Genome::ConnectionGene>>>;
        public:
            Genome(){}
            Genome(std::size_t numInputs, std::size_t numOutputs, bool canBeRecursive = true, bool cppn = false);
            /// @todo node is added, where an old connection was,
            ///       then the src to new node weight is 1 and the
            ///       new node to dest is equal to the old weight.
            /// *--*
            /// *-w = 1-*(new node)-old w-*
            void addNodeGene(const NodeGene& ng) noexcept;
            void addConnectionGene(const ConnectionGene& cg) noexcept;
            std::size_t getLastInnovationNode() const noexcept;
            std::size_t getLastInnovationConnection() const noexcept;
            /// will mutate adding a node + 2 connection or a connection between 2 existing nodes.
            void mutate() noexcept;
            /// look at src genome NEAT
            void mutateWeights(double power,double rate) noexcept;
            void mutateEnable() noexcept;
            ~Genome() = default;
            /// d = ((c1*E)/N) + ((c2*D)/N) + c3 * avg of W differences between genomes.
            /// c1,c2,c3 coefficients
            /// E Excess genes
            /// D Disjoint genes
            static double distance(const Genome& g1, const Genome& g2, 
                                    const double& c1 = 0.3, 
                                    const double& c2 = 0.2, 
                                    const double& c3 = 0.5) noexcept;
            static matchingChromosomes getMatchingChromosomes(const Genome& g1, const Genome& g2) noexcept;
            /// genes are randomly chosen from either parent at matching genes,
            /// whereas all excess or disjoint genes are always included from the more fit parent
            /// there’s a preset chance that an inherited gene is disabled if it is disabled in either parent
            static std::unique_ptr<Genome> reproduce(const Genome& g1, const Genome& g2) noexcept;
            static std::unique_ptr<NeuralNetwork> makePhenotype() noexcept;
            static double excessGenes(const Genome& g1, const Genome& g2) noexcept;
            static double disjointGenes(const Genome& g1, const Genome& g2) noexcept;
        private:
            std::size_t genomeID;
            std::size_t speciesID;
            double fitness;
            bool rnnPermited;
            std::vector<NodeGene> nodeChromosomes;
            std::vector<ConnectionGene> connectionChromosomes;
    };
}

#endif // #ifndef EVOAI_GENOME_HPP
