#include <EvoAI/Evaluators/ParameterMemoryEvaluator.hpp>

namespace EvoAI{
    void ParameterMemoryEvaluator::operator()(NeuralNetwork& net, NetworkMetrics& m) const noexcept{
        m.memoryBytes = static_cast<double>(net.getParameters().size() * sizeof(double));
    }
}
