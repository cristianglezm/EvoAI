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
             * @brief ///TODO Very Important
             * @return 
             */
            std::vector<Connection*>& getConnections();
            /**
             * @brief 
             * @param src
             * @param dest
             * @return 
             */
            Connection* findConnection(Link&& src,Link&& dest);
            /**
             * @brief 
             * @return 
             */
            bool hasConnection(Link&& src, Link&& dest);
            /**
             * @brief 
             * @return 
             */
            inline std::size_t size() const{ return layers.size(); }
            /**
             * @brief 
             */
            void reset();
            /**
             * @brief 
             */
            void resetContext();
            /**
             * @brief 
             */
            NeuronLayer& operator[](const std::size_t& index);
            friend class NNRenderer;
            ~NeuralNetwork() = default;
        private:
            /**
             * @brief applies the neuron activation
             * @param at ActivationType
             * @param n const Neuron&
             * @return const double
             */
            const double activate(NeuronLayer::ActivationType at, const Neuron& n) const;
        private:
            std::vector<NeuronLayer> layers;
            std::vector<Connection*> connections;
            bool connectionsCached;
            double mse;
    };
}
#endif // NEURAL_NETWORK_HPP
