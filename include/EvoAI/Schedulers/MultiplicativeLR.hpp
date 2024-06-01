#ifndef EVOAI_MULTIPLICATIVE_LR_HPP
#define EVOAI_MULTIPLICATIVE_LR_HPP

#include <JsonBox.h>
#include <EvoAI/Export.hpp>

namespace EvoAI{
    /** 
     * @brief MultiplicativeLR Scheduler Algorithm
     * @code
     *     auto mLR = MultiplicativeLR{0.1};
     * @endcode
     */
    struct EvoAI_API MultiplicativeLR final{
        /**
         * @brief constructor
         * @param factor
         */
        MultiplicativeLR(double factor = 0.1);
        /**
         * @brief constructor for JsonBox::Object
         * @param o JsonBox::Object
         */
        MultiplicativeLR(JsonBox::Object o);
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
        double m_factor;
    };
}

#endif // EVOAI_MULTIPLICATIVE_LR_HPP