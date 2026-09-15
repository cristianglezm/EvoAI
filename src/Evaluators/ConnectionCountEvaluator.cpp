#include <EvoAI/Evaluators/ConnectionCountEvaluator.hpp>

namespace EvoAI{
    void ConnectionCountEvaluator::operator()(NeuralNetwork& net, NetworkMetrics& m) const noexcept{
        m.numConnections = static_cast<double>(net.getConnections().size());
    }
}
