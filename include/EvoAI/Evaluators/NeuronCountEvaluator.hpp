#ifndef EVOAI_NEURON_COUNT_EVALUATOR_HPP
#define EVOAI_NEURON_COUNT_EVALUATOR_HPP

#include <EvoAI/Evaluators/NetworkMetrics.hpp>
#include <EvoAI/NeuralNetwork.hpp>
#include <EvoAI/Export.hpp>

namespace EvoAI{
    /**
     *  @brief fills NetworkMetrics::numNeurons with net.getNeurons().size()
     */
    struct EvoAI_API NeuronCountEvaluator{
        void operator()(NeuralNetwork& net, NetworkMetrics& m) const noexcept;
    };
}

#endif // EVOAI_NEURON_COUNT_EVALUATOR_HPP
