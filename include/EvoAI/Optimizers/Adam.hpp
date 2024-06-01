#ifndef EVOAI_OPTIMIZERS_ADAM_HPP
#define EVOAI_OPTIMIZERS_ADAM_HPP

#include <EvoAI/Export.hpp>
#include <JsonBox.h>

#include <vector>

namespace EvoAI{
    class Connection;
    /** @todo impl parallelism for updating weights?
     * @brief Adam Optimization Algorithm
     */
    struct EvoAI_API Adam final{
        /**
         * @brief constructor
         * @param parameters std::vector<Connection*>
         * @param maximize bool
         * @param beta1 double
         * @param beta2 double
         * @param epsilon double
         * @param accumGradients bool if true it won't average the gradients
         */
        explicit Adam(std::vector<Connection*>&& parameters, bool maximize = false, double beta1 = 0.9, double beta2 = 0.999, double epsilon = 1e-8, bool accumGradients = false);
        /**
         * @brief constructor to load from a JsonBox::Object
         * @param o JsonBox::Object
         * @param parameters std::vector<Connection*>
         */
        Adam(JsonBox::Object o, std::vector<Connection*>&& parameters);
        /**
         * @brief converts to JsonBox::Value
         * @return JsonBox::Value
         */
        JsonBox::Value toJson() const noexcept;
        /**
         * @brief update the weights
         * @param lr double learning rate
         * @param batchSize double batch size
         */
        void operator()(double lr, std::size_t batchSize) noexcept;
        /**
         * @brief resets the gradients to 0.0.
         */
        void zeroGrad() noexcept;
        ~Adam() = default;
        // data
        std::vector<Connection*> m_params;
        double m_beta1;
        double m_beta2;
        std::vector<double> m_mWeight;
        std::vector<double> m_vWeight;
        double m_epsilon;
        std::size_t m_t;
        bool m_maximize;
        bool m_accumGradients;
    };
}

#endif //EVOAI_OPTIMIZERS_ADAM_HPP