#ifndef EVOAI_OPTIMIZERS_SGD_HPP
#define EVOAI_OPTIMIZERS_SGD_HPP

#include <EvoAI/Export.hpp>
#include <JsonBox.h>

namespace EvoAI{
    class Connection;
    /** @todo impl parallelism for updating weights?
     * @brief Stochastic Gradient Descend Algorithm
     */
    struct EvoAI_API SGD final{
        /**
         * @brief constructor
         * @param parameters std::vector<Connection*>
         * @param momentum double
         * @param nesterov bool
         * @param maximize bool
         * @param accumGradients bool if true it won't average the gradients
         */
        explicit SGD(std::vector<Connection*>&& parameters, double momentum = 0.0, bool nesterov = false, bool maximize = false, bool accumGradients = false);
        /**
         * @brief constructor for JsonBox::Object
         * @param o JsonBox::Object
         * @param parameters std::vector<Connection*>
         */
        SGD(JsonBox::Object o, std::vector<Connection*>&& parameters);
        /**
         * @brief converts to a JsonBox::Value
         * @return JsonBox::Value
         */
        JsonBox::Value toJson() const noexcept;
        /**
         * @brief updates the weights
         * @param lr learning rate
         * @param batchSize batch size to average the gradients with.
         */
        void operator()(double lr, std::size_t batchSize) noexcept;
        /**
         * @brief resets the gradients to 0.0.
         */
        void zeroGrad() noexcept;
        // data
        bool m_nesterov;
        bool m_maximize;
        bool m_accumGradients;
        std::vector<Connection*> m_params;
        std::vector<double> m_momentumWeights;
        std::vector<double> m_velocityWeights;
    };
}

#endif //EVOAI_OPTIMIZERS_SGD_HPP