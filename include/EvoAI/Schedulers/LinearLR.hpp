#ifndef EVOAI_LINEAR_LR_HPP
#define EVOAI_LINEAR_LR_HPP

#include <JsonBox.h>
#include <EvoAI/Export.hpp>

namespace EvoAI{
    /**
     * @brief LinearLR Scheduler Algorithm
     *
     * @code
     *     // every 10 steps, lr -= 0.01, floor 0.0
     *     auto lLR = LinearLR{10, 0.01, 0.0};
     * @endcode
     */
    struct EvoAI_API LinearLR final{
        /**
         * @brief constructor
         * @param step std::size_t how often (in epochs) to apply decrement
         * @param decrement double
         * @param minValue double floor, lr will not go below this
         */
        LinearLR(std::size_t step = 10u, double decrement = 0.01, double minValue = 0.0);
        /**
         * @brief constructor for JsonBox::Object
         * @param o JsonBox::Object
         */
        LinearLR(JsonBox::Object o);
        /**
         * @brief converts to JsonBox::Value
         * @return JsonBox::Value
         */
        JsonBox::Value toJson() const noexcept;
        /**
         * @brief applies the scheduler to lr
         * @param lr
         * @param epoch
         */
        double operator()(double lr, std::size_t epoch) const noexcept;
        // data
        std::size_t m_step;
        double m_decrement;
        double m_minValue;
    };
}

#endif // EVOAI_LINEAR_LR_HPP
