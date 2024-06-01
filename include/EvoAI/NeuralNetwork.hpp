#ifndef EVOAI_NEURAL_NETWORK_HPP
#define EVOAI_NEURAL_NETWORK_HPP

#include <vector>
#include <cstdint>
#include <algorithm>

#include <EvoAI/Loss.hpp>
#include <EvoAI/NeuronLayer.hpp>
#include <EvoAI/Activations.hpp>
#include <EvoAI/Optimizers.hpp>
#include <EvoAI/Export.hpp>
#include <EvoAI/DataLoader.hpp>

#include <JsonBox.h>

namespace EvoAI{
    /**
     * @class NeuralNetwork
     * @author Cristian Glez <cristian.glez.m@gmail.com>
     * @date 01/01/2016
     * @file NeuralNetwork.hpp
     * @brief A Neural Network
     */
    class EvoAI_API NeuralNetwork final{
        public:
            using trainingFormat = std::vector<std::vector<double>>;
        public:
            /**
             * @brief default constructor
             */
            NeuralNetwork();
            /**
             * @brief This will build the basic structure without connections
             * @param numInputs std::size_t Inputs that the nn will take.
             * @param numHiddenLayers std::size_t Hidden layers that will be created.
             * @param numNeuronsPerHiddenLayer const std::vector<std::size_t>& neurons per hidden layer
             * @param numOutputs std::size_t number of outputs
             * @param bias double bias for the neural network, it will put the same for every layer.
             */
            NeuralNetwork(std::size_t numInputs,std::size_t numHiddenLayers,
                          const std::vector<std::size_t>& numNeuronsPerHiddenLayer, 
                          std::size_t numOutputs,double bias);
            /**
             * @brief Constructor for a json object.
             * @param o JsonBox::Object
             */
            NeuralNetwork(JsonBox::Object o);
            /**
             * @brief Load the NN from a json file
             * @param filename
             */
            NeuralNetwork(const std::string& filename);
            /**
             * @brief adds a layer to the neural network
             * @param l const NeuronLayer&
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
             * @return std::vector<double>
             */
            std::vector<double> run();
            /**
             * @brief calls setsInput and calls run
             * @param input const std::vector<double>&
             * @return const std::vector<double>&
             */
            std::vector<double> forward(const std::vector<double>& input) noexcept;
            /**
             * @brief calls setsInput and calls run
             * @param input std::vector<double>&&
             * @return std::vector<double>&&
             */
            std::vector<double> forward(std::vector<double>&& input) noexcept;
            /**
             * @brief calculates the gradients for the network.
             * @warning It needs to be called before calling reset as the outputs and gradients would be 0.0
             * @code
             *     // ...
             *     auto outputs = nn.forward({...});
             *     nn.backward(lossFn{}.backward(...));
             *     optim.step(epoch);
             *     nn.reset() // it will reset the network
             * @endcode
             * @param gradientLoss std::vector<double>&&
             * @return std::vector<double> gradients of layer[0]
             */
            std::vector<double> backward(std::vector<double>&& gradientLoss) noexcept;
            /**
             * @brief method to train the neural network.
             * @details
             *  The test function should be std::pair<double aka avgTest, double aka accuracy> testFn(NeuralNetwork&, DataLoader<Dataset>) noexcept;
             * @tparam Optim configured Optimizer
             * @tparam LossAlgo the loss function to use
             * @tparam Dataset dataset for training and testing
             * @tparam TestFn test function
             * @param trainingDataset DataLoader<Dataset>&
             * @param testDataset DataLoader<Dataset>&
             * @param optim Optimizer<Algo, SchedulerAlgo> Optimizer to apply updates for each batch
             * @param epoch epoches to do
             * @param lossAlgo LossAlgo
             * @param testFn TestFn
             * @return average loss of epoch, avg loss of test and accuracy
             */
            template<typename Optim, typename LossAlgo, class Dataset, typename TestFn>
            std::vector<std::vector<double>> train(DataLoader<Dataset>& trainingDataset, 
                                                    DataLoader<Dataset>& testDataset, 
                                                    Optim& optim, std::size_t epoch, LossAlgo&& lossAlgo,
                                                    TestFn&& testFn){
                std::vector<std::vector<double>> data(3);
                data[0].reserve(epoch);
                data[1].reserve(epoch);
                data[2].reserve(epoch);
                Loss::Loss<LossAlgo> lossFn{std::forward<LossAlgo>(lossAlgo)};
                auto totalLoss = 0.0;
                auto printAt = 1;
                std::cout << "globalStep: " << globalStep << std::endl;
                for(auto e=0u;e<epoch;++e){
                    if(e%printAt==0){
                        std::cout << "[ Epoch " << (e+1) << "/" << epoch << " ]\n---------------------\n";
                    }
                    auto samples = trainingDataset.size();
                    auto totalBatchLoss= 0.0;
                    auto batchSize = trainingDataset.getBatchSize();
                    for(auto i=0u;i<samples;++i){
                        for(auto b=0u;b<batchSize;++b){
                            auto [inputs, expectedOutputs] = trainingDataset();
                            auto outputs = forward(inputs);
                            auto loss = lossFn(expectedOutputs, outputs);
                            totalLoss += loss;
                            totalBatchLoss += loss;
                            backward(lossFn.backward(expectedOutputs, outputs));
                            reset();
                        }
                        if(e%printAt==0){
                            std::cout << "[ batch # " << ((i+1) * batchSize) << "/" << (samples * batchSize) << "] - [avgBatchLoss: " << (totalBatchLoss / batchSize) << "]\n";
                        }
                        totalBatchLoss = 0.0;
                        optim.step(e);
                        optim.zeroGrad();
                    }
                    trainingDataset.shuffle();
                    auto avgLoss = totalLoss / (samples * batchSize);
                    if(e%printAt==0){
                        std::cout << "\tavgLoss: " << avgLoss << std::endl;
                    }
                    totalLoss = 0.0;
                    auto [testAvgLoss, accuracy] = testFn(*this, testDataset);
                    testDataset.shuffle();
                    data[0].emplace_back(avgLoss);
                    data[1].emplace_back(testAvgLoss);
                    data[2].emplace_back(accuracy);
                    lastAvgLoss = avgLoss;
                    ++globalStep;
                }
                return data;
            }
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
             * @param n const Neuron&
             * @param layerIndex std::size_t
             * @return NeuralNetwork&
             */
            NeuralNetwork& addNeuron(const Neuron& n, std::size_t layerIndex);
            /**
             * @brief Removes a neuron and the connections it has.
             * @param n Neuron to remove
             * @return bool
             */
            bool removeNeuron(Neuron* n);
            /**
             * @brief gets the index of the layer and neuron from the neuron specified.
             * Returns a link with -1,-1 for not found
             * @param n
             * @return Link
             */
            Link getIndex(Neuron* n) const;
            /**
             * @brief sets the inputs returns true if succeeded, false if it failed.
             * @param ins std::vector<double>&&
             * @return bool
             */
            bool setInputs(std::vector<double>&& ins) noexcept;
            /**
             * @brief sets the inputs returns true if succeeded, false if it failed.
             * @param ins const std::vector<double>&
             * @return bool
             */
            bool setInputs(const std::vector<double>& ins) noexcept;
            /**
             * @brief Adds a Connection
             * @param c const Connection&
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
             * @param dest Link
             */
            void removeConnectionsWithDest(const Link& dest);
            /**
             * @brief Removes Connections with src
             * @param src Link&&
             */
            void removeConnectionsWithSrc(const Link& src);
            /**
             * @brief getter for the connections
             * @return std::vector<Connection*>&
             */
            std::vector<Connection*>& getConnections();
            /**
             * @brief getter for the connections + biases
             * @return std::vector<Connection*>
             */
            std::vector<Connection*> getParameters() noexcept;
            /**
             * @brief getter for the neurons
             * @return std::vector<Neuron*>&
             */
            std::vector<Neuron*>& getNeurons();
            /**
             * @brief finds a connection returns nullptr if not found.
             * @param src Link&&
             * @param dest Link&&
             * @return Connection* nullptr if not found
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
             * @brief resets the gradients of the connections
             */
            void resetConnections();
            /**
             * @brief Converts to a JsonBox::Value
             * @return JsonBox::Value
             */
            JsonBox::Value toJson() const;
            /**
             * @brief Writes the neural network json to a file.
             * @param filename
             */
            void writeToFile(const std::string& filename) const;
            /**
             * @brief writes a dot file for graphviz dot program.
             * @param filename const std::string&
             * @return bool
             */
            bool writeDotFile(const std::string& filename) noexcept;
            /**
             * @brief clears the Neural Network.
             */
            void clear();
            /**
             * @brief gives the last Average Loss
             * @return double lastAvgLoss
             */
            double getLastAvgLoss() const noexcept{ return lastAvgLoss; }
            /**
             * @brief gives the globalStep
             * @return const std::uint64_t globalStep
             */
            const std::uint64_t& getGlobalStep() const noexcept{ return globalStep; }
            /**
             * @brief direct access to layers and neurons
             * @code
             *     neuralnetwork[1][1].setBiasWeight(-0.3); // sets the biasWeight of layer 1 and neuron 1
             *     neuralnetwork[3][0].getOutput(); // gets output of layer 3 and neuron 0
             * @endcode
             * @warning Does not check if index is out of range.
             * @param index std::size_t index layer
             * @return NeuronLayer&
             */
            NeuronLayer& operator[](std::size_t index);
            /**
             * @brief return src and dest of the connection
             * @param conn EvoAI::Connection&
             * @code
             *     auto& [src, dest] = nn[conn];
             * @endcode
             * @return std::pair<Neuron&, Neuron&>
             */
            std::pair<Neuron&, Neuron&> operator[](Connection& conn) noexcept;
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
             * @return double
             */
            double activate(Neuron::ActivationType at, const Neuron& n);
            /**
             * @brief Applies the neuron derivative
             * @param at Neuron::ActivationType
             * @param n const Neuron&
             * @return double
             */
            double derivate(Neuron::ActivationType at,const Neuron& n);
        private:
            std::vector<NeuronLayer> layers;
            mutable std::vector<Connection*> connections;
            mutable std::vector<Neuron*> neurons;
            mutable bool connectionsCached;
            mutable bool neuronsCached;
            std::uint64_t globalStep;
            double lastAvgLoss;
    };
}
#endif // EVOAI_NEURAL_NETWORK_HPP
