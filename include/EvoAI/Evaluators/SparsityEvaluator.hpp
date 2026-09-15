#ifndef EVOAI_SPARSITY_EVALUATOR_HPP
#define EVOAI_SPARSITY_EVALUATOR_HPP

#include <functional>

#include <EvoAI/Evaluators/NetworkMetrics.hpp>
#include <EvoAI/NeuralNetwork.hpp>
#include <EvoAI/Export.hpp>

namespace EvoAI{
     /**
      * @brief Estimates network sparsity and stores it in NetworkMetrics::sparsity.
      *
      * @details
      * Sparsity is calculated as:
      *
      *   1 - (numConnections / maxPossibleConnections)
      *
      * By default, `maxPossibleConnections` is `numNeurons * (numNeurons - 1)`,
      * representing the maximum number of directed connections between distinct
      * neurons. A custom maximum can be supplied through a callback.
      *
      * The result is clamped to the range [0.0, 1.0]. This accounts for networks
      * containing duplicate connections or self-loops, which may cause the actual
      * connection count to exceed the default maximum.
      *
      * The connection and neuron counts are computed directly from `net` rather
      * than read from other metrics, so this evaluator is independent of evaluator
      * ordering and composition.
      */
    class EvoAI_API SparsityEvaluator final{
        public:
            /**
             *  @brief constructor
             *  @param maxPossible optional override for the "max possible
             *         connections" denominator; defaults to
             *         numNeurons*(numNeurons-1) (i.e. pass nullptr, the
             *         default) when omitted.
             */
            explicit SparsityEvaluator(std::function<double(NeuralNetwork&)> maxPossible = nullptr);
            /**
             *  @brief writes m.sparsity, clamped to [0.0, 1.0]. If
             *  maxPossibleConnections is 0 (e.g. a 0- or 1-neuron network),
             *  writes 0.0 rather than dividing by zero; if numConnections
             *  exceeds maxPossibleConnections writes 0.0 rather than a negative
             *  value.
             *  @param net NeuralNetwork&
             *  @param m NetworkMetrics&
             */
            void operator()(NeuralNetwork& net, NetworkMetrics& m) const;
            ~SparsityEvaluator() = default;
        private:
            std::function<double(NeuralNetwork&)> maxPossibleConnectionsFn;
    };
}

#endif // EVOAI_SPARSITY_EVALUATOR_HPP
