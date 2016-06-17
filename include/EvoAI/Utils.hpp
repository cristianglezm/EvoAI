#ifndef EVOAI_UTILS_HPP
#define EVOAI_UTILS_HPP

#include <memory>
#include <utility>
#include <random>
#include <chrono>
#include <string>

#include <EvoAI/NeuralNetwork.hpp>

namespace EvoAI{
    /**
     * @brief returns a random number between min and max.
     * @param min const double&
     * @param max const double&
     * @return double
     */
    double random(const double& min, const double& max);
    /**
     * @brief Returns a random number between min and max.
     * @param min const int&
     * @param max const int&
     * @return int
     */
    int random(const int& min, const int& max);
    /**
     * @brief gives a random true or false given the rate.
     * @param rate
     * @return bool
     */
    bool doAction(float rate) noexcept;
    /**
     * @brief Combines two hashes
     * @param seed std::size_t
     * @param v const T& hasheable Object
     * @return std::size_t
     */
    template <class T>
    inline std::size_t hashCombine(std::size_t seed, const T& v) noexcept;
    /**
     * @brief normalize values from one range to another
     * @code
     * normalize<float>(50,-1,1,0,100);
     * @endcode
     * @tparam number type
     * @param val T
     * @param normMin T
     * @param normMax T
     * @param min T
     * @param max T
     * @return T
     */
    template<typename T = float>
    inline T normalize(const T& val, const T& normMin, const T& normMax, const T& min, const T& max) noexcept;
    /**
     * @brief Creates a FeedForward Neural Network.
     * @param numInputs
     * @param numHidden
     * @param numNeuronsPerHidden
     * @param numOutputs
     * @param bias
     * @return std::unique_ptr<NeuralNetwork>
     */
    std::unique_ptr<NeuralNetwork> createFeedForwardNN(const size_t& numInputs, const size_t& numHidden,const size_t& numNeuronsPerHidden, const std::size_t& numOutputs, const double& bias);
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
    std::unique_ptr<NeuralNetwork> createElmanNeuralNetwork(const std::size_t& numInputs, const std::size_t& numHidden,
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
    std::unique_ptr<NeuralNetwork> createFullyConnectedCPPN(const std::size_t& numInputs, const std::size_t& numHidden,
                                                            const std::size_t& numNeuronsPerHiddenLayer, const std::size_t& numOutputs, const double& bias);
    /**
     * @brief Creates a CPPN with random activations functions and some random connections.
     * @param numInputs number of inputs
     * @param numHidden number of hidden layers
     * @param numNeuronsPerHiddenLayer number of neurons per hidden layer
     * @param numOutputs number of outputs
     * @param bias bias
     * @return std::unique_ptr<NeuralNetwork>
     */
    std::unique_ptr<NeuralNetwork> createCPPN(const std::size_t& numInputs, const std::size_t& numHidden,
                                                const std::size_t& numNeuronsPerHiddenLayer, const std::size_t& numOutputs, const double& bias);
    template<typename T = float>
    inline T normalize(const T& val,const T& normMin, const T& normMax, const T& min, const T& max) noexcept{
        return (normMin + (val-min)*(normMax-normMin)/(max-min));
    }
    template <class T>
    inline std::size_t hashCombine(std::size_t seed, const T& v) noexcept{
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
        return seed;
    }
}
#endif // EVOAI_UTILS_HPP