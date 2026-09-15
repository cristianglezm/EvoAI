#ifndef EVOAI_CONNECTION_COUNT_EVALUATOR_HPP
#define EVOAI_CONNECTION_COUNT_EVALUATOR_HPP

#include <EvoAI/Evaluators/NetworkMetrics.hpp>
#include <EvoAI/NeuralNetwork.hpp>
#include <EvoAI/Export.hpp>

namespace EvoAI{
    /**
     *  @brief fills NetworkMetrics::numConnections with net.getConnections().size()
     */
    struct EvoAI_API ConnectionCountEvaluator{
        void operator()(NeuralNetwork& net, NetworkMetrics& m) const noexcept;
    };
}

#endif // EVOAI_CONNECTION_COUNT_EVALUATOR_HPP
