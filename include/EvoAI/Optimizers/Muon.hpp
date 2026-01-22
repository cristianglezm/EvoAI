#ifndef EVOAI_OPTIMIZERS_MUON_HPP
#define EVOAI_OPTIMIZERS_MUON_HPP

#include <EvoAI/Export.hpp>
#include <JsonBox.h>

#include <vector>

namespace EvoAI{
    class Connection;
    /** @brief Muon Optimization Algorithm
     *
     * Hybrid optimizer: momentum on top of RMS-style scaling.
     */
    struct EvoAI_API Muon final{
        /**
         * @brief constructor
         * @param parameters std::vector<Connection*>
         * @param maximize bool
         * @param mu double momentum coefficient (default 0.9)
         * @param alpha double RMS decay (default 0.99)
         * @param epsilon double
         * @param accumGradients bool if true it won't average the gradients
         */
        explicit Muon(std::vector<Connection*>&& parameters, bool maximize = false, double mu = 0.9, double alpha = 0.99, double epsilon = 1e-8, bool accumGradients = false);
        /**
         * @brief constructor to load from a JsonBox::Object
         * @param o JsonBox::Object
         * @param parameters std::vector<Connection*>
         */
        Muon(JsonBox::Object o, std::vector<Connection*>&& parameters);
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
        ~Muon() = default;
        // data
        std::vector<Connection*> m_params;
        double m_mu;                  // momentum coefficient
        double m_alpha;               // rms decay
        std::vector<double> m_velocity;
        std::vector<double> m_rms;
        double m_epsilon;
        bool m_maximize;
        bool m_accumGradients;
    };
}

#endif //EVOAI_OPTIMIZERS_MUON_HPP
