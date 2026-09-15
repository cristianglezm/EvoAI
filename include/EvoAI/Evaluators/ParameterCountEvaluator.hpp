#ifndef EVOAI_PARAMETER_COUNT_EVALUATOR_HPP
#define EVOAI_PARAMETER_COUNT_EVALUATOR_HPP

#include <EvoAI/Evaluators/NetworkMetrics.hpp>
#include <EvoAI/NeuralNetwork.hpp>
#include <EvoAI/Export.hpp>

namespace EvoAI{
    /**
     *  @brief fills NetworkMetrics::parameterCount with
     *  net.getParameters().size() (connections + biases)
     */
    struct EvoAI_API ParameterCountEvaluator{
        void operator()(NeuralNetwork& net, NetworkMetrics& m) const noexcept;
    };
}

#endif // EVOAI_PARAMETER_COUNT_EVALUATOR_HPP
