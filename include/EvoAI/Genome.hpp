#ifndef GENOME_HPP
#define GENOME_HPP

#include <vector>
#include <memory>
#include <utility>
#include <random>
#include <string>

#include <EvoAI/NeuralNetwork.hpp>
#include <JsonBox.h>

namespace EvoAI{
    /* TODO
    class Genome final{
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
            Genome();
            Genome(const std::string& filename);
            JsonBox::Value toJson() const;
            Genome& setMutationRate(const float& rate);
            bool writeToFile(const std::string& filename) const;
            ~Genome() = default;
        private:
            Genome& addConnection(const Connection& c);
            Genome& removeConnection(const Connection& c);
            Genome& addNeuron(const Neuron& n);
            Genome& removeNeuron(const Neuron& n);
            bool initChromosomes();
        private:
            int genomeID;
            int speciesID;
            double fitness;
            std::vector<NodeGene> nodeChromosomes;
            std::vector<ConnectionGene> connectionChromosomes;
            std::unique_ptr<NeuralNetwork> phenotype;
    };
    /// d = ((c1*E)/N) + ((c2*D)/N) + c3 * avg of W differences between genomes.
    double distance(Genome& g1,Genome& g2);
    */
}
#endif // GENOME_HPP
