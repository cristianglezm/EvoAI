#include <EvoAI/Evaluators/NeuronCountEvaluator.hpp>

namespace EvoAI{
    void NeuronCountEvaluator::operator()(NeuralNetwork& net, NetworkMetrics& m) const noexcept{
        m.numNeurons = static_cast<double>(net.getNeurons().size());
    }
}
