#ifndef EVOAI_LOSS_MEAN_ABSOLUTE_ERROR_HPP
#define EVOAI_LOSS_MEAN_ABSOLUTE_ERROR_HPP

#include <vector>
#include <cmath>
#include <EvoAI/Export.hpp>

namespace EvoAI{
   namespace Loss{
        /**
         * @brief Mean Absolute Error
         */
        struct EvoAI_API MeanAbsoluteError{
            /**
             * @brief forward step to calculate loss
             * @param expectedOutputs std::vector<double>&
             * @param outputs std::vector<double>&
             * @return double
             */
            double operator()(const std::vector<double>& expectedOutputs, const std::vector<double>& outputs) noexcept;
            /**
             * @brief backward step to calculate gradients
             * @param expectedOutputs std::vector<double>&
             * @param outputs std::vector<double>&
             * @return std::vector<double> gradients for output layer
             */
            std::vector<double> backward(const std::vector<double>& expectedOutputs, const std::vector<double>& outputs) noexcept;
        };
   }
}
#endif // EVOAI_LOSS_MEAN_ABSOLUTE_ERROR_HPP