#ifndef EVOAI_COMPOSITE_EVALUATOR_HPP
#define EVOAI_COMPOSITE_EVALUATOR_HPP

#include <tuple>
#include <utility>

#include <EvoAI/Evaluators/NetworkMetrics.hpp>
#include <EvoAI/Utils/TypeUtils.hpp>

namespace EvoAI{
    /**
     *  @brief Runs a sequence of MetricEvaluators over a Network, folding
     *  the results into a single Metrics breakdown.
     *  @tparam Network the type being evaluated (e.g. EvoAI::NeuralNetwork)
     *  @tparam Metrics the result type each Evaluator writes into, defaults
     *          to EvoAI::NetworkMetrics
     *  @tparam Evaluators each needs to fulfill
     *          meta::metric_evaluator_v<Evaluator, Network, Metrics>, i.e.
     *          have a member function void operator()(Network&, Metrics&) const
     *  @details
     *   CompositeEvaluator only concerns itself with network cost/complexity
     *   metrics (connections, latency, memory, ...). Task performance is
     *   specific to the problem being solved, so it isn't produced by an
     *   Evaluator here, set Metrics::taskScore directly after calling
     *   this, then hand the Metrics to an ObjectivePolicy.
     *  @warning template argument deduction can't fill in Evaluators... if
     *   Network/Metrics are given explicitly (they're not deducible from
     *   the constructor arguments), use EvoAI::makeCompositeEvaluator
     *   instead of naming this type directly unless you're spelling out
     *   every template argument yourself.
     */
    template<class Network, class Metrics = NetworkMetrics, class... Evaluators>
    class CompositeEvaluator final{
            static_assert((meta::metric_evaluator_v<Evaluators, Network, Metrics> && ...),
                "Every Evaluator needs to fulfill EvoAI::meta::metric_evaluator_v<Evaluator, Network, Metrics>, more info at Evaluators/CompositeEvaluator.hpp");
        public:
            /**
             *  @brief constructor
             *  @param evals Evaluators... each fulfilling meta::metric_evaluator_v<Evaluator, Network, Metrics>
             */
            explicit CompositeEvaluator(Evaluators... evals)
            : evaluators(std::move(evals)...){}
            /**
             *  @brief runs every Evaluator over net, in construction order,
             *  folding results into a single Metrics.
             *  @param net Network&
             *  @return Metrics
             */
            Metrics operator()(Network& net) const{
                Metrics metrics{};
                std::apply([&net, &metrics](const auto&... evals){ (evals(net, metrics), ...); }, evaluators);
                return metrics;
            }
            ~CompositeEvaluator() = default;
        private:
            std::tuple<Evaluators...> evaluators;
    };
    /**
     * @brief Convenience factory for constructing a CompositeEvaluator.
     *
     * Explicit template arguments can be used to specify the network type, while
     * the evaluator types are deduced from the function arguments.
     *
     * @code
     * auto evaluator = EvoAI::makeCompositeEvaluator<EvoAI::NeuralNetwork>(
     *     EvoAI::ConnectionCountEvaluator{}, EvoAI::NeuronCountEvaluator{});
     * @endcode
     *
     * @tparam Network the type being evaluated
     * @tparam Metrics the metrics type; defaults to EvoAI::NetworkMetrics
     * @tparam Evaluators the evaluator types, deduced from the arguments
     * @param evaluators the evaluators to combine
     * @return a CompositeEvaluator configured with the specified network, metrics,
     *         and evaluators
     */
    template<class Network, class Metrics = NetworkMetrics, class... Evaluators>
    CompositeEvaluator<Network, Metrics, Evaluators...> makeCompositeEvaluator(Evaluators&&... evaluators){
        return CompositeEvaluator<Network, Metrics, Evaluators...>(std::forward<Evaluators>(evaluators)...);
    }
}

#endif // EVOAI_COMPOSITE_EVALUATOR_HPP
