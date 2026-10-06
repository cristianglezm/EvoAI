#ifndef EVOAI_SOMEVALUATION_HPP
#define EVOAI_SOMEVALUATION_HPP

#include <EvoAI/Export.hpp>

#include <cstddef>
#include <optional>
#include <vector>

namespace EvoAI{
    /**
     * @brief how well a map fits a set of samples, see SelfOrganizingMap::evaluate().
     */
    struct EvoAI_API SOMEvaluation final{
        std::size_t width = 0u;
        std::size_t height = 0u;
        /// mean distance from each usable sample to its best matching unit (0 when no sample was usable)
        double meanQuantizationError = 0.0;
        /// fraction of usable samples whose two closest nodes are not neighbours, empty unless it was asked for
        std::optional<double> topographicError;
        /// samples won by each node, flat index y * width + x
        std::vector<std::size_t> nodeCounts;
        std::size_t samplesUsed = 0u;
        std::size_t samplesSkipped = 0u;
        /**
         * @brief nodes that won at least one sample
         */
        std::size_t usedNodes() const noexcept;
        /**
         * @brief samples won by the node that won the most
         */
        std::size_t busiestNodeCount() const noexcept;
        /**
         * @brief usedNodes() / number of nodes, 0 for an empty evaluation
         */
        double fractionUsed() const noexcept;
    };
}

#endif // EVOAI_SOMEVALUATION_HPP
