#include <EvoAI/SOMEvaluation.hpp>

#include <algorithm>

namespace EvoAI{
    std::size_t SOMEvaluation::usedNodes() const noexcept{
        return static_cast<std::size_t>(std::count_if(nodeCounts.begin(), nodeCounts.end(), [](auto count){
            return count > 0u;
        }));
    }
    std::size_t SOMEvaluation::busiestNodeCount() const noexcept{
        return nodeCounts.empty() ? 0u : *std::max_element(nodeCounts.begin(), nodeCounts.end());
    }
    double SOMEvaluation::fractionUsed() const noexcept{
        return nodeCounts.empty() ? 0.0 : static_cast<double>(usedNodes()) / static_cast<double>(nodeCounts.size());
    }
}
