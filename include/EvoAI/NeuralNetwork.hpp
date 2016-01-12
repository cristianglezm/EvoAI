#ifndef NEURAL_NETWORK_HPP
#define NEURAL_NETWORK_HPP

#include <vector>
#include <algorithm>
#include <EvoAI/NeuronLayer.hpp>
#include <EvoAI/Activations.hpp>
#include <JsonBox.h>

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
             * @brief default constructor
             * @return 
             */
            NeuralNetwork();
            /**
             * @brief This will build the basic structure without connections
             * @param numInputs const std::size_t& Inputs that the nn will take.
             * @param numHiddenLayers const std::size_t& Hidden layers that will be created.
             * @param numNeuronsPerHiddenLayer const std::size_t& neuron per hidden layer
             * @param numOutputs const std::size_t& number of outputs
             * @param bias const double& bias fot he neural network, it will put the same for every layer.
             * @return NeuronNetwork
             */
            NeuralNetwork(const std::size_t& numInputs,const std::size_t& numHiddenLayers,
                          const std::size_t& numNeuronsPerHiddenLayer,const std::size_t& numOutputs,const double& bias);
            /**
             * @brief Load the NN from a json file
             * @param filename
             * @return NeuralNetwork
             */
            NeuralNetwork(const std::string& filename);
            /**
             * @brief adds a layer to the neural network
             * @param l
             * @return NeuralNetwork&
             */
            NeuralNetwork& addLayer(const NeuronLayer& l);
            /**
             * @brief 
             * @param l
             * @return 
             */
            bool removeLayer(const NeuronLayer& l);
            /**
             * @brief 
             * @return 
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
             * @brief adds a neuron to a specific layer
             * @param n
             * @return 
             */
            NeuralNetwork& addNeuron(const Neuron& n, const std::size_t& layerIndex);
            /**
             * @brief Removes a neuron and the connections it has.
             * @param n Neuron to remove
             * @return bool
             */
            bool removeNeuron(Neuron* n);
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
             * @brief Removes Connections with dest
             * @param dest Link&&
             */
            void removeConnectionsWithDest(Link&& dest);
            /**
             * @brief Removes Connections with src
             * @param src Link&&
             */
            void removeConnectionsWithSrc(Link&& src);
            /**
             * @brief 
             * @return std::vector<Connection*>&
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
             * @brief Returns how many layers it has.
             * @return std::size_t
             */
            inline std::size_t size() const{ return layers.size(); }
            /**
             * @brief resets the neurons that are not Neuron::Type::CONTEXT
             */
            void reset();
            /**
             * @brief resets the contexts neurons.
             */
            void resetContext();
            /**
             * @brief Use this to get a JsonBox::Value
             * @return JsonBox::Value
             */
            JsonBox::Value toJson() const;
            /**
             * @brief Writes the neural network json to a file.
             * @param filename
             */
            void writeToFile(const std::string& filename) const;
            /**
             * @brief 
             */
            void clear();
            /**
             * @brief direct access to layers and neurons
             * Examples: neuralnetwork[1][1].setBiasWeight(-0.3); // sets the biasWeight of neuron 1 from layer 1
             *          neuralnetwork[3][0].getOutput(); // gets output of neuron 0 from layer 3
             * Does not check if index is out of range
             * @param index const std::size_t& index layer
             */
            NeuronLayer& operator[](const std::size_t& index);
            /**
             * @brief compares two neural networks
             * @param rhs
             */
            bool operator==(const NeuralNetwork& rhs) const;
            friend class NNRenderer;
            ~NeuralNetwork() = default;
        private:
            /**
             * @brief Applies the neuron activation
             * @param at ActivationType
             * @param n const Neuron&
             * @return const double
             */
            const double activate(NeuronLayer::ActivationType at, const Neuron& n);
        private:
            std::vector<NeuronLayer> layers;
            std::vector<Connection*> connections;
            bool connectionsCached;
            double mse;
    };
}
#endif // NEURAL_NETWORK_HPP
