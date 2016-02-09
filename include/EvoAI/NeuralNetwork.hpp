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
            using trainingFormat = std::vector<std::vector<double>>;
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
             * @brief Removes a layer
             * @param l const NeuronLayer&
             * @return bool
             */
            bool removeLayer(const NeuronLayer& l);
            /**
             * @brief Process the NeuralNetwork
             * @return 
             */
            std::vector<double> run();
            /**
             * @brief method to train the neural network.
             * @param inputs std::vector<std::vector<double>>
             * @param expectedOutput std::vector<std::vector<double>>
             * @param learningRate
             * @param epoch
             */
            void train(std::vector<std::vector<double>>&& inputs,std::vector<std::vector<double>>&& expectedOutputs, const double& learningRate, const double& momentum, const int& epoch);
            /**
             * @brief Sets the neural network layers.
             * @return NeuralNetwork&
             */
            NeuralNetwork& setLayers(std::vector<NeuronLayer>&& lys);
            /**
             * @brief returns the layers of the neural network.
             * @return std::vector<NeuronLayer>&
             */
            inline std::vector<NeuronLayer>& getLayers(){ return layers; }
            /**
             * @brief adds a neuron to a specific layer
             * @param n
             * @return NeuralNetwork&
             */
            NeuralNetwork& addNeuron(const Neuron& n, const std::size_t& layerIndex);
            /**
             * @brief Removes a neuron and the connections it has.
             * @param n Neuron to remove
             * @return bool
             */
            bool removeNeuron(Neuron* n);
            /**
             * @brief sets the inputs returns true if succeded, false if it failed.
             * @return bool
             */
            bool setInputs(std::vector<double>&& ins);
            /**
             * @brief Adds a Connection
             * @param c
             * @return NeuralNetwork&
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
             * @brief getter for the connections
             * @return std::vector<Connection*>&
             */
            std::vector<Connection*>& getConnections();
            /**
             * @brief getter for the neurons
             * @return std::vector<Neuron*>&
             */
            std::vector<Neuron*>& getNeurons();
            /**
             * @brief finds a connection returns nullptr if not found.
             * @param src
             * @param dest
             * @return Connection*
             */
            Connection* findConnection(Link&& src,Link&& dest);
            /**
             * @brief Checks if a connection exists.
             * @return bool
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
             * @brief resets the neurons that are Neuron::Type::CONTEXT
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
             * @brief clears the Neural Network.
             */
            void clear();
            /**
             * @brief direct access to layers and neurons
             * Examples: neuralnetwork[1][1].setBiasWeight(-0.3); // sets the biasWeight of layer 1 and neuron 1
             *          neuralnetwork[3][0].getOutput(); // gets output of layer 3 and neuron 0
             * Does not check if index is out of range.
             * @param index const std::size_t& index layer
             */
            NeuronLayer& operator[](const std::size_t& index);
            /**
             * @brief gives the last MSE
             * @return const double& MSE
             */
            const double& getMSE() const noexcept{ return mse; }
            /**
             * @brief compares two neural networks
             * @param rhs
             */
            bool operator==(const NeuralNetwork& rhs) const;
            ~NeuralNetwork() = default;
        private:
            /**
             * @brief Applies the neuron activation
             * @param at Neuron::ActivationType
             * @param n const Neuron&
             * @return const double
             */
            const double activate(Neuron::ActivationType at, const Neuron& n);
            /**
             * @brief Applies the neuron derivative
             * @param at Neuron::ActivationType
             * @param n const Neuron&
             * @return const double
             */
            const double derivate(Neuron::ActivationType at,const Neuron& n);
            /**
             * @brief resets the gradients of the connections
             */
            void resetConnections();
        private:
            std::vector<NeuronLayer> layers;
            std::vector<Connection*> connections;
            std::vector<Neuron*> neurons;
            bool connectionsCached;
            bool neuronsCached;
            double mse;
    };
}
#endif // NEURAL_NETWORK_HPP
