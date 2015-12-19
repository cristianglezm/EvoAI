#ifndef NEURAL_NETWORK_HPP
#define NEURAL_NETWORK_HPP

#include <vector>
#include <EvoAI/NeuronLayer.hpp>

namespace EvoAI{
    class NeuralNetwork final{
        public:
            NeuralNetwork();
            NeuralNetwork(const std::size_t& numInputs,const std::size_t& numHiddenLayers,
                          const std::size_t& numNeuronsPerHiddenLayer,const std::size_t& numOutputs);
            NeuralNetwork& addLayer(const NeuronLayer& l);
            NeuralNetwork& removeLayer(const NeuronLayer& l);
            NeuralNetwork& setLayers(std::vector<NeuronLayer>&& lys);
            std::vector<NeuronLayer>& getLayers();
            ~NeuralNetwork();
        private:
            std::vector<NeuronLayer> layers;
    };
}
#endif // NEURAL_NETWORK_HPP
