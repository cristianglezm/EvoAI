#ifndef EVOAI_NEAT_HPP
#define EVOAI_NEAT_HPP

#include <vector>
#include <memory>
#include <utility>
#include <random>
#include <string>

#include <EvoAI/NeuralNetwork.hpp>
#include <JsonBox.h>

namespace EvoAI{
    /// @todo define and implement
    template<bool cppn = false>
    class Neat final{
        public:
            struct NodeGene{
                public:
                    NodeGene();
                    ~NodeGene() = default;
                private:
                    int layerID;
                    int neuronID;
                    Neuron::ActivationType actType;
            };
            class ConnectionGene{
                public:
                    ConnectionGene();
                    ~ConnectionGene() = default;
                private:
                    int innovationID;
                    Connection c;
                    bool enabled;
            };
        public:
            Neat();
            Neat(const std::string& filename);
            JsonBox::Value toJson() const noexcept;
            Neat& setMutationRate(const float& rate) noexcept;
            bool writeToFile(const std::string& filename) const;
            ~Neat() = default;
        private:
            Neat& addConnection(const Connection& c) noexcept;
            Neat& removeConnection(const Connection& c) noexcept;
            Neat& addNeuron(const Neuron& n) noexcept;
            Neat& removeNeuron(const Neuron& n) noexcept;
        private:
            int64_t genomeID;
            int64_t speciesID;
            double fitness;
            double mutationRate;
            std::vector<NodeGene> nodeChromosomes;
            std::vector<ConnectionGene> connectionChromosomes;
            std::unique_ptr<NeuralNetwork> phenotype;
    };
    /// d = ((c1*E)/N) + ((c2*D)/N) + c3 * avg of W differences between genomes.
    template<bool cppn = false>
    double distance(Neat<cppn>& n1,Neat<cppn>& n2) noexcept;
}
#endif // EVOAI_NEAT_HPP