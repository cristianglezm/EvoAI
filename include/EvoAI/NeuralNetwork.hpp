#ifndef NEURAL_NETWORK_HPP
#define NEURAL_NETWORK_HPP

#include <vector>
#include <EvoAI/NeuronLayer.hpp>

namespace EvoAI{
    /**
     *
     */
    class NeuralNetwork final{
        public:
            /**
             *
             */
            NeuralNetwork();
            /**
             *
             */
            NeuralNetwork(const std::size_t& numInputs,const std::size_t& numHiddenLayers,
                          const std::size_t& numNeuronsPerHiddenLayer,const std::size_t& numOutputs,const double bias);
            /**
             *
             */
            NeuralNetwork& addLayer(const NeuronLayer& l);
            /**
             *
             */
            bool removeLayer(const NeuronLayer& l);
            /**
             *
             */
            std::vector<double> run(auto ActivationFunction);
            /**
             *
             */
            NeuralNetwork& setLayers(std::vector<NeuronLayer>&& lys);
            /**
             *
             */
            inline std::vector<NeuronLayer>& getLayers(){ return layers; }
            /**
             *
             */
            NeuralNetwork& setInputs(std::vector<double>&& ins);
            /**
             *
             */
            NeuralNetwork& addConnection(const Connection& c);
            bool removeConnection(const Connection& c);
            ~NeuralNetwork() = default;
        private:
            std::vector<NeuronLayer> layers;
            double bias;
    };
    std::vector<double> NeuralNetwork::run(auto ActivationFunction){
    }
}
#endif // NEURAL_NETWORK_HPP
