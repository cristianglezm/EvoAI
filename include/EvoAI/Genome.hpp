#ifndef GENOME_HPP
#define GENOME_HPP

#include <vector>
#include <memory>
#include <utility>
#include <random>
#include <string>

#include <EvoAI/NeuralNetwork.hpp>
#include <JsonBox.h>

/// TODO
/// initial values
/// funciones de la nn modifica values hidden layers, hidden neurons, connections, chromoSize?
/// species size chromosomes
/// limite cost function
/// serializar nn, genome
namespace EvoAI{
    class Genome final{
        public:
            Genome();
            Genome(const std::size_t& chromoSize);
            Genome(const std::string& filename);
            bool update();
            JsonBox::Value toJson();
            ~Genome() = default;
        private:
            Genome& addConnection();
            Genome& removeConnection();
            Genome& addNeuron();
            Genome& removeNeuron();
            Genome& addNeuronLayer();
            Genome& removeNeuronLayer();
            Genome& addChromosome(const double& chromo);
            Genome& removeChromosome(const double& chromo);
            bool initChromosomes();
        private:
            std::vector<double> chromosomes;
            std::unique_ptr<NeuralNetwork> nn;
            double fitness;
            int speciesId;
    };
}
#endif // GENOME_HPP
