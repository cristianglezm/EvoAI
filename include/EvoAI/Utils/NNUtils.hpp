#ifndef EVOAI_NN_UTILS_HPP
#define EVOAI_NN_UTILS_HPP

#include <memory>
#include <utility>
#include <random>
#include <chrono>
#include <string>

#include <EvoAI/NeuralNetwork.hpp>
#include <EvoAI/UTils/RandomUtils.hpp>
#include <EvoAI/Export.hpp>

namespace EvoAI{
    /**
     * @brief Creates a FeedForward Neural Network.
     * @param numInputs
     * @param numHidden
     * @param numNeuronsPerHidden
     * @param numOutputs
     * @param bias
     * @return std::unique_ptr<NeuralNetwork>
     */
    EvoAI_API std::unique_ptr<NeuralNetwork> createFeedForwardNN(const size_t& numInputs, const size_t& numHidden,
                                                                        const size_t& numNeuronsPerHidden, const std::size_t& numOutputs, const double& bias);
    /**
     * @brief Creates an Elman Neural Network.
     * 
     * This will create a Neural Network with a context layer per every hidden layer.
     * 
     * @param numInputs number of inputs
     * @param numHidden number of hidden layers
     * @param numNeuronsPerHiddenLayer number of neurons per hidden layer
     * @param numOutputs number of outputs
     * @param bias bias
     * @return std::unique_ptr<NeuralNetwork>
     */
    EvoAI_API std::unique_ptr<NeuralNetwork> createElmanNeuralNetwork(const std::size_t& numInputs, const std::size_t& numHidden,
                                                            const std::size_t& numNeuronsPerHiddenLayer, const std::size_t& numOutputs, const double& bias);
    /**
     * @brief Creates a fully connected CPPN with random activations functions.
     * @param numInputs number of inputs
     * @param numHidden number of hidden layers
     * @param numNeuronsPerHiddenLayer number of neurons per hidden layer
     * @param numOutputs number of outputs
     * @param bias bias
     * @return std::unique_ptr<NeuralNetwork>
     */
    EvoAI_API std::unique_ptr<NeuralNetwork> createFullyConnectedCPPN(const std::size_t& numInputs, const std::size_t& numHidden,
                                                            const std::size_t& numNeuronsPerHiddenLayer, const std::size_t& numOutputs, const double& bias);
    /** @todo fix segmentation fault [last 2 lines]
     * @brief Creates a CPPN with random activations functions and some random connections.
     * @param numInputs number of inputs
     * @param numHidden number of hidden layers
     * @param numNeuronsPerHiddenLayer number of neurons per hidden layer
     * @param numOutputs number of outputs
     * @param bias bias
     * @return std::unique_ptr<NeuralNetwork>
     */
    EvoAI_API std::unique_ptr<NeuralNetwork> createCPPN(const std::size_t& numInputs, const std::size_t& numHidden,
                                                const std::size_t& numNeuronsPerHiddenLayer, const std::size_t& numOutputs, const double& bias);
}

#endif // EVOAI_NN_UTILS_HPP