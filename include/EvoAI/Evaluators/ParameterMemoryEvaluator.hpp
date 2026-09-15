#ifndef EVOAI_PARAMETER_MEMORY_EVALUATOR_HPP
#define EVOAI_PARAMETER_MEMORY_EVALUATOR_HPP

#include <EvoAI/Evaluators/NetworkMetrics.hpp>
#include <EvoAI/NeuralNetwork.hpp>
#include <EvoAI/Export.hpp>

namespace EvoAI{
    /**
      * @brief Estimates the network's parameter memory usage and stores it in
      *        NetworkMetrics::memoryBytes.
      *
      * @details
      * The estimate is calculated as `net.getParameters().size() *
      * sizeof(double)`. It does not account for allocator overhead, object
      * bookkeeping, or other platform-specific memory usage.
      *
      * For OS-level or otherwise hardware-specific measurements, implement a
      * custom MetricEvaluator.
      */
    struct EvoAI_API ParameterMemoryEvaluator{
        void operator()(NeuralNetwork& net, NetworkMetrics& m) const noexcept;
    };
}

#endif // EVOAI_PARAMETER_MEMORY_EVALUATOR_HPP
