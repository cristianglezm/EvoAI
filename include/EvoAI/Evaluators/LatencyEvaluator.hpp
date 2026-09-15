#ifndef EVOAI_LATENCY_EVALUATOR_HPP
#define EVOAI_LATENCY_EVALUATOR_HPP

#include <chrono>
#include <cstddef>
#include <algorithm>
#include <utility>

namespace EvoAI{
    /**
     * @brief Measures the execution time of a single inference pass and stores it
     *        in Metrics::latency, in seconds.
     *
     * @tparam Network the type being evaluated
     * @tparam InferenceFn a callable with `void operator()(Network&) const` that
     *         performs a single inference pass
     *
     * @details
     * LatencyEvaluator is independent of the metrics type. Its call operator is
     * templated so it can be composed with any CompositeEvaluator whose metrics
     * type provides a `double latency` field.
     *
     * The evaluator performs warmup runs before measuring latency. Warmup runs are
     * discarded because initial calls may be slower due to allocator and cache
     * warmup, or one-time setup performed by the Network implementation.
     *
     * @param measuredRuns the number of inference runs to time; values less than
     *        one are treated as one
     */
    template<class Network, class InferenceFn>
    class LatencyEvaluator final{
        public:
            /**
             *  @brief constructor
             *  @param fn InferenceFn a callable running one inference pass
             *  @param warmup std::size_t discarded runs before timing starts, defaults to 0
             *  @param measured std::size_t timed runs to average over, defaults to 1
             */
            explicit LatencyEvaluator(InferenceFn fn, std::size_t warmup = 0, std::size_t measured = 1)
            : inferenceFn(std::move(fn))
            , warmupRuns(warmup)
            , measuredRuns(std::max<std::size_t>(1u, measured)){}
            /**
             *  @brief runs the warmup + measured inference passes and
             *  writes the average per-run latency in seconds to m.latency.
             *  @param net Network&
             *  @param m Metrics& any type with a `double latency` field
             */
            template<class Metrics>
            void operator()(Network& net, Metrics& m) const{
                for(auto i = 0u; i < warmupRuns; ++i){
                    inferenceFn(net);
                }
                auto start = std::chrono::steady_clock::now();
                for(auto i = 0u; i < measuredRuns; ++i){
                    inferenceFn(net);
                }
                auto end = std::chrono::steady_clock::now();
                m.latency = std::chrono::duration<double>(end - start).count() / static_cast<double>(measuredRuns);
            }
            ~LatencyEvaluator() = default;
        private:
            InferenceFn inferenceFn;
            std::size_t warmupRuns;
            std::size_t measuredRuns;
    };
}

#endif // EVOAI_LATENCY_EVALUATOR_HPP
