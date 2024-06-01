#ifndef EVOAI_EXPONENTIAL_LR_HPP
#define EVOAI_EXPONENTIAL_LR_HPP

#include <JsonBox.h>
#include <EvoAI/Export.hpp>

namespace EvoAI{
    /**
     * @brief ExponentialLR Scheduler Algorithm
     * @param m_gamma double
     * @code
     *     auto eLR = ExponentialLR{0.5};
     * @endcode
     */
    struct EvoAI_API ExponentialLR final{
        /**
         * @brief constructor
         * @param gamma
         */
        ExponentialLR(double gamma = 0.1);
        /**
         * @brief constructor for JsonBox::Object
         * @param o JsonBox::Object
         */
        ExponentialLR(JsonBox::Object o);
        /**
         * @brief converts to a JsonBox::Value
         * @return JsonBox::Value
         */
        JsonBox::Value toJson() const noexcept;
        /**
         * @brief applies the scheduling to lr
         * @param lr
         * @param epoch
         */
        double operator()(double lr, std::size_t epoch) const noexcept;
        // data
        double m_gamma;
    };
}

#endif // EVOAI_EXPONENTIAL_LR_HPP