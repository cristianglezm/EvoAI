#ifndef EVOAI_NETWORK_METRICS_HPP
#define EVOAI_NETWORK_METRICS_HPP

#include <JsonBox.h>
#include <EvoAI/Export.hpp>

namespace EvoAI{
    /**
     * @brief Metrics collected by a CompositeEvaluator for a network.
     *
     * Each metric is stored as a named field so callers can inspect the
     * individual measurements that contribute to a network's fitness.
     *
     * @details
     * `taskScore` and `fitness` are not populated by MetricEvaluators:
     *
     * - `taskScore` is provided by the caller because task performance is
     *   specific to the problem being solved.
     * - `fitness` is computed by an ObjectivePolicy, such as
     *   WeightedSumObjective, which combines `taskScore` with the cost metrics
     *   collected by the evaluators.
     */
    struct EvoAI_API NetworkMetrics{
        double taskScore = 0.0;
        double latency = 0.0; ///< seconds
        double memoryBytes = 0.0;
        double numConnections = 0.0;
        double numNeurons = 0.0;
        double parameterCount = 0.0;
        double sparsity = 0.0;
        double fitness = 0.0;
        /**
         *  @brief converts to a JsonBox::Value, for logging/inspecting a
         *  generation's metrics rather than round-tripping evolved state.
         *  @return JsonBox::Value
         */
        JsonBox::Value toJson() const noexcept;
    };
}

#endif // EVOAI_NETWORK_METRICS_HPP
