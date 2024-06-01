#ifndef EVOAI_LOSS_HPP
#define EVOAI_LOSS_HPP

#include <vector>
#include <cmath>
#include <EvoAI/Export.hpp>
#include <EvoAI/Utils/TypeUtils.hpp>

#include <EvoAI/Loss/MeanError.hpp>
#include <EvoAI/Loss/MeanAbsoluteError.hpp>
#include <EvoAI/Loss/MeanSquaredError.hpp>
#include <EvoAI/Loss/BinaryCrossEntropy.hpp>
#include <EvoAI/Loss/MultiClassCrossEntropy.hpp>

namespace EvoAI{
   namespace Loss{
        template<class Algo>
        /**
         * @brief Algo needs to fulfill these requirements: <br />
         *      Algo has a double operator()(const std::vector<double>& expectedOutputs, const std::vector<double>& outputs) noexcept <br />
         *      Algo has a std::vector<double> backward(const std::vector<double>& expectedOutputs, const std::vector<double>& outputs) noexcept <br />
         * @tparam Algo
         */
        struct Loss{
            static_assert(meta::is_a_loss_v<Algo>, "Algo needs to be a Loss Algo, more info at Loss.hpp");
            /**
             * @brief constructor
             * @param algo Algo struct implementing is_a_loss_v
             */
            Loss(Algo&& algo)
            : loss{algo}{}
            /**
             * @brief forward step to calculate loss
             * @param expectedOutputs std::vector<double>&
             * @param outputs std::vector<double>&
             * @return double
             */
            double operator()(const std::vector<double>& expectedOutputs, const std::vector<double>& outputs) noexcept{
                return loss(expectedOutputs, outputs);
            }
            /**
             * @brief backward step to calculate gradients
             * @param expectedOutputs std::vector<double>&
             * @param outputs std::vector<double>&
             * @return std::vector<double> gradients for output layer
             */
            std::vector<double> backward(const std::vector<double>& expectedOutputs, const std::vector<double>& outputs) noexcept{
                return loss.backward(expectedOutputs, outputs);
            }
            // algorithm
            Algo loss;
        };
   }
}
#endif // EVOAI_LOSS_HPP
