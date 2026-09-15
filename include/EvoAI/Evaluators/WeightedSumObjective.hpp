#ifndef EVOAI_WEIGHTED_SUM_OBJECTIVE_HPP
#define EVOAI_WEIGHTED_SUM_OBJECTIVE_HPP

#include <vector>

#include <EvoAI/Evaluators/NetworkMetrics.hpp>

namespace EvoAI{
    /**
     * @brief Describes one penalty applied to a metric when computing fitness.
     *
     * The penalty contributes `coefficient * (metric / target)` to the total
     * penalty.
     *
     * @tparam Metrics the metrics type containing the selected metric
     */
    template<class Metrics = NetworkMetrics>
    struct PenaltyTerm{
        double Metrics::* metric; ///< Metric to penalize.
        double target;            ///< Normalization target for the metric.
        double coefficient;       ///< Weight applied to the normalized metric.
    };

    /**
     * @brief Computes fitness as task score minus a weighted sum of metric costs.
     *
     * @tparam Metrics the metrics type; defaults to EvoAI::NetworkMetrics
     */
    template<class Metrics = NetworkMetrics>
    class WeightedSumObjective final{
        public:
            /**
             * @brief Constructs an objective with the specified penalty terms.
             *
             * @param terms the metric penalties to apply when computing fitness
             */
            explicit WeightedSumObjective(std::vector<PenaltyTerm<Metrics>> terms)
            : penalties(std::move(terms)){}
            /**
             * @brief Computes and stores the fitness for a set of metrics.
             *
             * Fitness is calculated as:
             *
             *     taskScore - sum(coefficient * (metric / target))
             *
             * `taskScore` is read from `m` and is expected to have been populated by
             * the caller. Each penalty with a positive `target` contributes to the
             * result; penalties with a non-positive target are ignored.
             *
             * The computed fitness is stored in `m.fitness` and returned.
             *
             * @param m the metrics containing the task score and metric values
             * @return the computed fitness, which is also stored in `m.fitness`
             */
            double operator()(Metrics& m) const noexcept{
                double fit = m.taskScore;
                for(const auto& p : penalties){
                    if(p.target > 0.0){
                        fit -= p.coefficient * ((m.*(p.metric)) / p.target);
                    }
                }
                m.fitness = fit;
                return fit;
            }
            ~WeightedSumObjective() = default;
        private:
            std::vector<PenaltyTerm<Metrics>> penalties;
    };
}

#endif // EVOAI_WEIGHTED_SUM_OBJECTIVE_HPP
