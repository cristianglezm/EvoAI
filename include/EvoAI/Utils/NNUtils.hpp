#ifndef EVOAI_NN_UTILS_HPP
#define EVOAI_NN_UTILS_HPP

#include <memory>
#include <utility>
#include <random>
#include <chrono>
#include <string>
#include <algorithm>
#include <numeric>

#include <EvoAI/NeuralNetwork.hpp>
#include <EvoAI/Utils/RandomUtils.hpp>
#include <EvoAI/Export.hpp>

namespace EvoAI{
    /**
     * @brief Creates a FeedForward Neural Network. weight init default Xavier uniform
     * @param numInputs
     * @param numHidden
     * @param numNeuronsPerHidden
     * @param numOutputs
     * @param bias
     * @return std::unique_ptr<NeuralNetwork>
     */
    EvoAI_API std::unique_ptr<NeuralNetwork> createFeedForwardNN(const size_t& numInputs, const size_t& numHidden,
                                                                        const std::vector<size_t>& numNeuronsPerHidden, std::size_t numOutputs, double bias);
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
    EvoAI_API std::unique_ptr<NeuralNetwork> createElmanNeuralNetwork(std::size_t numInputs, std::size_t numHidden,
                                                            const std::vector<std::size_t>& numNeuronsPerHiddenLayer, std::size_t numOutputs, double bias);
    /**
     * @brief Creates a fully connected CPPN with random activations functions.
     *        weight init default Xavier uniform
     * @param numInputs number of inputs
     * @param numHidden number of hidden layers
     * @param numNeuronsPerHiddenLayer number of neurons per hidden layer
     * @param numOutputs number of outputs
     * @param bias bias
     * @return std::unique_ptr<NeuralNetwork>
     */
    EvoAI_API std::unique_ptr<NeuralNetwork> createFullyConnectedCPPN(std::size_t numInputs, std::size_t numHidden,
                                                            const std::vector<std::size_t>& numNeuronsPerHiddenLayer, std::size_t numOutputs, double bias);
    /**
     * @brief Creates a CPPN with random activations functions and some random connections.
     * @param numInputs number of inputs
     * @param numHidden number of hidden layers
     * @param numNeuronsPerHiddenLayer number of neurons per hidden layer
     * @param numOutputs number of outputs
     * @param bias bias
     * @return std::unique_ptr<NeuralNetwork>
     */
    EvoAI_API std::unique_ptr<NeuralNetwork> createCPPN(std::size_t numInputs, std::size_t numHidden,
                                                const std::vector<std::size_t>& numNeuronsPerHiddenLayer, std::size_t numOutputs, double bias);
    /**
     * @brief Uniform initialization uses randomGen().random(-1.0, 1.0, layerSize)
     * @param nn NeuralNetwork&
     */
    EvoAI_API void UniformInit(NeuralNetwork& nn) noexcept;
    /**
     * @brief LeCun initialization uniform distribution
     * @param nn NeuralNetwork&
     */
    EvoAI_API void LeCunInit(NeuralNetwork& nn) noexcept;
    /**
     * @brief LeCun initialization normal distribution
     * @param nn NeuralNetwork&
     */
    EvoAI_API void LeCunInitNormal(NeuralNetwork& nn) noexcept;
    /**
     * @brief He initialization uniform distribution
     * @param nn NeuralNetwork&
     */
    EvoAI_API void HeInit(NeuralNetwork& nn) noexcept;
    /**
     * @brief He initialization normal distribution
     * @param nn NeuralNetwork&
     */
    EvoAI_API void HeInitNormal(NeuralNetwork& nn) noexcept;
    /**
     * @brief Xavier initialization uniform distribution
     * @param nn NeuralNetwork&
     */
    EvoAI_API void XavierInit(NeuralNetwork& nn) noexcept;
    /**
     * @brief Xavier initialization normal distribution
     * @param nn NeuralNetwork&
     */
    EvoAI_API void XavierInitNormal(NeuralNetwork& nn) noexcept;
    /**
     * @brief write data to a file to render with tools/showMultiLinePlot.py
     * @param filename std::string_view filename to save info into.
     * @param legend std::vector<std::string> legend for the plots
     * @param data std::vector<std::vector<double>> data for the plot
     */
    EvoAI_API void writeMultiPlot(std::string_view filename, const std::vector<std::string>& legend, const std::vector<std::vector<double>>& data) noexcept;
    /**
     * @brief gives the index of maximum value
     * @param outputs const std::vector<double>&
     * @return std::size index of maximum value
     */
    EvoAI_API std::size_t Argmax(const std::vector<double>& outputs) noexcept;
    /**
     * @brief gives the index of maximum value
     * @param outputs NeuronLayer&
     * @return std::size_t index of maximum value
     */
    EvoAI_API std::size_t Argmax(NeuronLayer& outputs) noexcept;
    /**
     * @brief gives the index of maximum value of given range (0 to size()-1)
     * @tparam Interator 
     * @param begin Interator 
     * @param end Interator 
     * @return std::size_t index of maximum value in given range.
     */
    template<typename Iterator>
    std::size_t Argmax(Iterator begin, Iterator end) noexcept{
        return std::distance(begin, std::max_element(begin, end));
    }
    /**
     * @brief gives the index of maximum value of given range (0 to size()-1) using Fn as comparator.
     * @tparam Iterator 
     * @tparam Fn Comparator
     * @param begin Iterator
     * @param end Iterator
     * @param fn Fn
     * @return std::size_t index of maximum value in given range
     */
    template<typename Iterator, typename Fn>
    std::size_t Argmax(Iterator begin, Iterator end, Fn&& fn) noexcept{
        return std::distance(begin, std::max_element(begin, end, fn));
    }
}

#endif // EVOAI_NN_UTILS_HPP
