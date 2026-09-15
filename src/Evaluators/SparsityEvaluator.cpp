#include <algorithm>

#include <EvoAI/Evaluators/SparsityEvaluator.hpp>

namespace EvoAI{
    SparsityEvaluator::SparsityEvaluator(std::function<double(NeuralNetwork&)> maxPossible)
    : maxPossibleConnectionsFn(std::move(maxPossible)){}

    void SparsityEvaluator::operator()(NeuralNetwork& net, NetworkMetrics& m) const{
        auto numConnections = static_cast<double>(net.getConnections().size());
        auto numNeurons = static_cast<double>(net.getNeurons().size());
        auto maxPossible = maxPossibleConnectionsFn ? maxPossibleConnectionsFn(net) : (numNeurons * (numNeurons - 1.0));
        m.sparsity = (maxPossible > 0.0) ? std::max(0.0, 1.0 - (numConnections / maxPossible)) : 0.0;
    }
}
