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
                          const std::size_t& numNeuronsPerHiddenLayer,const std::size_t& numOutputs,const double& bias);
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
             * @brief 
             * @return 
             */
            NeuralNetwork& setLayers(std::vector<NeuronLayer>&& lys);
            /**
             * @brief 
             * @return 
             */
            inline std::vector<NeuronLayer>& getLayers(){ return layers; }
            /**
             * @brief 
             * @return 
             */
            bool setInputs(std::vector<double>&& ins);
            /**
             * @brief 
             * @param c
             * @return 
             */
            NeuralNetwork& addConnection(const Connection& c);
            /**
             * @brief Removes a connection from the neural network.
             * @param c Connection&
             * @return true if removed successfully.
             */
            bool removeConnection(Connection& c);
            ~NeuralNetwork() = default;
        private:
            std::vector<NeuronLayer> layers;
    };
    std::vector<double> NeuralNetwork::run(auto ActivationFunction){
        return std::vector<double>();
    }
}
#endif // NEURAL_NETWORK_HPP
