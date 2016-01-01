#ifndef NEURAL_NETWORK_HPP
#define NEURAL_NETWORK_HPP

#include <vector>
#include <algorithm>
#include <EvoAI/NeuronLayer.hpp>
#include <EvoAI/Activations.hpp>

namespace EvoAI{
    /**
     * @class NeuralNetwork
     * @author Cristian Glez <cristian.glez.m@gmail.com>
     * @date 01/01/2016
     * @file NeuralNetwork.hpp
     * @brief 
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
            std::vector<double> run();
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
            /**
             * @brief 
             * @return 
             */
            inline std::size_t size() const{ return layers.size(); }
            /**
             * @brief ///TODO Very Important
             * @return 
             */
            std::vector<Connection*>& getConnections();
            /**
             * @brief 
             */
            NeuronLayer& operator[](const std::size_t& index);
            friend class NNRenderer;
            ~NeuralNetwork() = default;
        private:
            std::vector<NeuronLayer> layers;
            std::vector<Connection*> connections;
            bool connectionsCached;
    };
}
#endif // NEURAL_NETWORK_HPP
