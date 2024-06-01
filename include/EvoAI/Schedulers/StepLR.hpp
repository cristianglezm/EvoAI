#ifndef EVOAI_STEP_LR_HPP
#define EVOAI_STEP_LR_HPP

#include <JsonBox.h>
#include <EvoAI/Export.hpp>

namespace EvoAI{
    /**
     * @brief StepLR Scheduler Algorithm
     * @code
     *     auto sLR = StepLR{10,0.1}; // every 10 steps will update lr
     * @endcode
     */
    struct EvoAI_API StepLR final{
        /**
         * @brief constructor
         * @param step std::size_t
         * @param gamma double
         */
        StepLR(std::size_t step = 10u, double gamma = 0.1);
        /**
         * @brief constructor for JsonBox::Object
         * @param o JsonBox::Object
         */
        StepLR(JsonBox::Object o);
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
        double m_gamma;
    };
}

#endif // EVOAI_STEP_LR_HPP