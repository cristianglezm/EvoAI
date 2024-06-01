#ifndef EVOAI_CONSTANT_LR_HPP
#define EVOAI_CONSTANT_LR_HPP

#include <JsonBox.h>
#include <EvoAI/Export.hpp>

namespace EvoAI{
    /**
     * @brief ConstantLR Scheduler Algorithm
     * @details Learning rate will not change
     */
    struct EvoAI_API ConstantLR final{
        /**
         * @brief default constructor
         */
        ConstantLR() = default;
        /**
         * @brief constructor for JsonBox::Object
         * @param o JsonBox::Object
         */
        ConstantLR(JsonBox::Object o);
        /**
         * @brief converts to JsonBox::Value
         * @return JsonBox::Value
         */
        JsonBox::Value toJson() const noexcept;
        /**
         * @brief applies the scheduling to lr
         * @param lr
         * @param epoch
         */
        double operator()(double lr, std::size_t epoch) const noexcept;
    };
}

#endif // EVOAI_CONSTANT_LR_HPP