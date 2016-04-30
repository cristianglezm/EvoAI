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
    /* TODO
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
            JsonBox::Value toJson() const;
            Neat& setMutationRate(const float& rate);
            bool writeToFile(const std::string& filename) const;
            ~Neat() = default;
        private:
            Neat& addConnection(const Connection& c);
            Neat& removeConnection(const Connection& c);
            Neat& addNeuron(const Neuron& n);
            Neat& removeNeuron(const Neuron& n);
        private:
            int genomeID;
            int speciesID;
            double fitness;
            double mutationRate;
            std::vector<NodeGene> nodeChromosomes;
            std::vector<ConnectionGene> connectionChromosomes;
            std::unique_ptr<NeuralNetwork> phenotype;
    };
    /// d = ((c1*E)/N) + ((c2*D)/N) + c3 * avg of W differences between genomes.
    double distance(Neat& g1,Neat& g2);
    */
}
#endif // EVOAI_NEAT_HPP