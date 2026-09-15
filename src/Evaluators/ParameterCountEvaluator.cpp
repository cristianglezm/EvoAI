#include <EvoAI/Evaluators/ParameterCountEvaluator.hpp>

namespace EvoAI{
    void ParameterCountEvaluator::operator()(NeuralNetwork& net, NetworkMetrics& m) const noexcept{
        m.parameterCount = static_cast<double>(net.getParameters().size());
    }
}
