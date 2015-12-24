#ifndef UTILS_HPP

#include <memory>
#include <utility>
#include <EvoAI/NeuralNetwork.hpp>

namespace EvoAI{
    /// TODO
    std::unique_ptr<NeuralNetwork> CreateFeedForwardNN(){ return std::make_unique<NeuralNetwork>(); }
}
#endif // UTILS_HPP
