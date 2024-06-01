#ifndef EVOAI_MULTI_STEP_LR_HPP
#define EVOAI_MULTI_STEP_LR_HPP

#include <vector>
#include <JsonBox.h>
#include <EvoAI/Export.hpp>

namespace EvoAI{
    /**
     * @brief MultiStepLR Scheduler Algorithm
     * @code
     *     auto msLR = MultiStepLR{{10,20,50,55}, 0.1};
     * @endcode
     */
    struct EvoAI_API MultiStepLR final{
        /**
         * @brief Constructor
         * @param steps std::vector<std::size_t>&&
         * @param gamma double
         */
        MultiStepLR(std::vector<std::size_t>&& steps, double gamma = 0.1);
        /**
         * @brief constructor for JsonBox::Object
         * @param o JsonBox::Object
         */
        MultiStepLR(JsonBox::Object o);
        /**
         * @brief converts to a JsonBox::Value
         * @return JsonBox::Value
         */
        JsonBox::Value toJson() const noexcept;
        /**
         * @brief applies the scheduling to lr
         * @param lr double
         * @param epoch std::size_t
         */
        double operator()(double lr, std::size_t epoch) const noexcept;
        //data
        std::vector<std::size_t> m_steps;
        double m_gamma;
        mutable std::size_t m_counter;
    };
}

#endif // EVOAI_MULTI_STEP_LR_HPP