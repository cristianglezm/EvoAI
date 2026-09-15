#ifndef EVOAI_EVALUATORS_TEST_HPP
#define EVOAI_EVALUATORS_TEST_HPP

#include <gtest/gtest.h>
#include <EvoAI/Evaluators.hpp>
#include <EvoAI.hpp>

namespace EvoAI{
    namespace Test{
        namespace{
            // input(2) -> hidden(3) -> output(1) = 6 neurons, no connections yet.
            NeuralNetwork makeTestNetwork() noexcept{
                NeuralNetwork nn(2,1,{3},1,1.0);
                nn.addConnection(Connection(Link(0,0), Link(1,0), 0.1));
                nn.addConnection(Connection(Link(0,1), Link(1,1), 0.2));
                nn.addConnection(Connection(Link(1,0), Link(2,0), 0.3));
                nn.addConnection(Connection(Link(1,1), Link(2,0), 0.4));
                return nn;
            }
        }
        TEST(EvaluatorsTest, ConnectionCountEvaluator){
            auto nn = makeTestNetwork();
            NetworkMetrics m{};
            ConnectionCountEvaluator{}(nn, m);
            EXPECT_EQ(m.numConnections, 4.0);
        }
        TEST(EvaluatorsTest, NeuronCountEvaluator){
            auto nn = makeTestNetwork();
            NetworkMetrics m{};
            NeuronCountEvaluator{}(nn, m);
            EXPECT_EQ(m.numNeurons, 6.0);
        }
        TEST(EvaluatorsTest, ParameterCountEvaluator){
            auto nn = makeTestNetwork();
            NetworkMetrics m{};
            ParameterCountEvaluator{}(nn, m);
            // 4 connections + 6 neuron biases, none frozen by default.
            EXPECT_EQ(m.parameterCount, 10.0);
        }
        TEST(EvaluatorsTest, CompositeEvaluatorFillsOnlyWhatItsToldTo){
            auto nn = makeTestNetwork();
            auto evaluator = makeCompositeEvaluator<NeuralNetwork>(
                ConnectionCountEvaluator{}, NeuronCountEvaluator{}, ParameterCountEvaluator{});
            auto metrics = evaluator(nn);
            EXPECT_EQ(metrics.numConnections, 4.0);
            EXPECT_EQ(metrics.numNeurons, 6.0);
            EXPECT_EQ(metrics.parameterCount, 10.0);
            // taskScore/latency/memoryBytes/sparsity/fitness are the
            // caller's or an ObjectivePolicy's responsibility, not
            // CompositeEvaluator's, confirm it leaves them untouched.
            EXPECT_EQ(metrics.taskScore, 0.0);
            EXPECT_EQ(metrics.latency, 0.0);
            EXPECT_EQ(metrics.memoryBytes, 0.0);
            EXPECT_EQ(metrics.sparsity, 0.0);
            EXPECT_EQ(metrics.fitness, 0.0);
        }
        TEST(EvaluatorsTest, NetworkMetricsToJson){
            NetworkMetrics m{};
            m.taskScore = 1.0;
            m.numConnections = 4.0;
            auto v = m.toJson();
            EXPECT_EQ(v["taskScore"].getDouble(), 1.0);
            EXPECT_EQ(v["numConnections"].getDouble(), 4.0);
        }
        TEST(EvaluatorsTest, LatencyEvaluatorRunsWarmupPlusMeasuredRuns){
            auto nn = makeTestNetwork();
            int calls = 0;
            auto inference = [&calls](NeuralNetwork&){ ++calls; };
            LatencyEvaluator<NeuralNetwork, decltype(inference)> latencyEval{inference, /*warmup*/2, /*measured*/5};
            NetworkMetrics m{};
            latencyEval(nn, m);
            EXPECT_EQ(calls, 7); // 2 warmup + 5 measured
            EXPECT_GE(m.latency, 0.0);
        }
        TEST(EvaluatorsTest, LatencyEvaluatorClampsZeroMeasuredRunsToOne){
            auto nn = makeTestNetwork();
            int calls = 0;
            auto inference = [&calls](NeuralNetwork&){ ++calls; };
            LatencyEvaluator<NeuralNetwork, decltype(inference)> latencyEval{inference, /*warmup*/0, /*measured*/0};
            NetworkMetrics m{};
            latencyEval(nn, m);
            EXPECT_EQ(calls, 1); // measuredRuns=0 is clamped to 1, not a division by zero
        }
        TEST(EvaluatorsTest, LatencyEvaluatorComposesWithStructuralEvaluators){
            auto nn = makeTestNetwork();
            auto inference = [](NeuralNetwork& net){
                net.forward({1.0, 0.0});
                net.reset();
            };
            auto evaluator = makeCompositeEvaluator<NeuralNetwork>(
                ConnectionCountEvaluator{}, LatencyEvaluator<NeuralNetwork, decltype(inference)>{inference, 1, 3});
            auto metrics = evaluator(nn);
            EXPECT_EQ(metrics.numConnections, 4.0);
            EXPECT_GE(metrics.latency, 0.0);
        }
        TEST(EvaluatorsTest, WeightedSumObjectiveAppliesPenalties){
            NetworkMetrics m{};
            m.taskScore = 100.0;
            m.numConnections = 10.0; // target 20 -> normalized 0.5, coefficient 0.1 -> -0.05
            m.latency = 2e-4;        // target 1e-4 -> normalized 2.0, coefficient 0.1 -> -0.2
            WeightedSumObjective<> objective({
                {&NetworkMetrics::numConnections, 20.0, 0.1},
                {&NetworkMetrics::latency, 1e-4, 0.1}
            });
            auto fitness = objective(m);
            EXPECT_NEAR(fitness, 100.0 - 0.05 - 0.2, 1e-9);
            EXPECT_NEAR(m.fitness, fitness, 1e-9); // also written back onto Metrics
        }
        TEST(EvaluatorsTest, WeightedSumObjectiveSkipsNonPositiveTargets){
            NetworkMetrics m{};
            m.taskScore = 10.0;
            m.numConnections = 999.0; // would divide by zero if not guarded
            WeightedSumObjective<> objective({
                {&NetworkMetrics::numConnections, 0.0, 0.5}
            });
            EXPECT_NEAR(objective(m), 10.0, 1e-9); // penalty skipped, not NaN/inf
        }
        TEST(EvaluatorsTest, WeightedSumObjectiveEndToEndWithCompositeEvaluator){
            auto nn = makeTestNetwork();
            auto evaluator = makeCompositeEvaluator<NeuralNetwork>(ConnectionCountEvaluator{});
            auto metrics = evaluator(nn);
            metrics.taskScore = 50.0; // caller-owned, not written by CompositeEvaluator
            WeightedSumObjective<> objective({{&NetworkMetrics::numConnections, 20.0, 0.1}});
            EXPECT_NEAR(objective(metrics), 50.0 - 0.1 * (4.0 / 20.0), 1e-9);
        }
        TEST(EvaluatorsTest, ObjectivePolicyTraitRejectsWrongSignature){
            struct NotAnObjective{};
            static_assert(!meta::objective_policy_v<NotAnObjective, NetworkMetrics>,
                "NotAnObjective has no matching operator() and must fail the trait");
            static_assert(meta::objective_policy_v<WeightedSumObjective<>, NetworkMetrics>,
                "WeightedSumObjective<> must fulfill the trait");
        }
        TEST(EvaluatorsTest, SparsityEvaluatorClampsNegativeToZero){
            NeuralNetwork nn(2,0,{},1,1.0); // 3 neurons -> max = 3*2 = 6
            nn.addConnection(Connection(Link(0,0), Link(0,0), 0.1)); // self-loop
            nn.addConnection(Connection(Link(0,0), Link(1,0), 0.1));
            nn.addConnection(Connection(Link(0,0), Link(1,0), 0.2)); // duplicate
            nn.addConnection(Connection(Link(0,0), Link(1,0), 0.3)); // duplicate
            nn.addConnection(Connection(Link(0,1), Link(1,0), 0.1));
            nn.addConnection(Connection(Link(0,1), Link(1,0), 0.2)); // duplicate
            nn.addConnection(Connection(Link(0,1), Link(1,0), 0.3)); // duplicate
            // 7 connections, max = 6 -- would be 1 - 7/6 = -1/6 without the floor
            NetworkMetrics m{};
            SparsityEvaluator{}(nn, m);
            EXPECT_GE(m.sparsity, 0.0);
            EXPECT_EQ(m.sparsity, 0.0);
        }
        TEST(EvaluatorsTest, ParameterMemoryEvaluatorEstimate){
            auto nn = makeTestNetwork();
            NetworkMetrics m{};
            ParameterMemoryEvaluator{}(nn, m);
            // 4 connections + 6 biases = 10 parameters, none frozen by default
            EXPECT_EQ(m.memoryBytes, 10.0 * sizeof(double));
        }
        TEST(EvaluatorsTest, SparsityEvaluatorDefaultsToNeuronCountTimesNeuronCountMinusOne){
            auto nn = makeTestNetwork();
            NetworkMetrics m{};
            SparsityEvaluator{}(nn, m);
            // 4 connections, 6 neurons -> 1 - 4/(6*5)
            EXPECT_NEAR(m.sparsity, 1.0 - (4.0 / 30.0), 1e-9);
        }
        TEST(EvaluatorsTest, SparsityEvaluatorAcceptsACustomDenominator){
            auto nn = makeTestNetwork();
            NetworkMetrics m{};
            SparsityEvaluator withCustomMax{[](NeuralNetwork&){ return 8.0; }};
            withCustomMax(nn, m);
            EXPECT_NEAR(m.sparsity, 1.0 - (4.0 / 8.0), 1e-9);
        }
        TEST(EvaluatorsTest, SparsityEvaluatorIsSelfContainedRegardlessOfEvaluatorOrder){
            auto nn = makeTestNetwork();
            auto evaluator = makeCompositeEvaluator<NeuralNetwork>(
                SparsityEvaluator{}, ConnectionCountEvaluator{}, NeuronCountEvaluator{});
            auto metrics = evaluator(nn);
            EXPECT_NEAR(metrics.sparsity, 1.0 - (4.0 / 30.0), 1e-9);
        }
        namespace{
            // A fully independent metrics type - no relationship to
            // NetworkMetrics at all - to prove the compile-time interface
            // is the extension point.
            struct MyMetrics{
                double layerCount = 0.0;
            };
            struct LayerCountEvaluator{
                void operator()(NeuralNetwork& net, MyMetrics& m) const noexcept{
                    m.layerCount = static_cast<double>(net.size());
                }
            };
        }
        TEST(EvaluatorsTest, CompositeEvaluatorWorksWithAUserDefinedMetricsType){
            auto nn = makeTestNetwork();
            auto evaluator = makeCompositeEvaluator<NeuralNetwork, MyMetrics>(LayerCountEvaluator{});
            auto metrics = evaluator(nn);
            EXPECT_EQ(metrics.layerCount, 3.0); // input, hidden, output
        }
        TEST(EvaluatorsTest, MetricEvaluatorTraitRejectsWrongSignature){
            struct NotAnEvaluator{};
            static_assert(!meta::metric_evaluator_v<NotAnEvaluator, NeuralNetwork, NetworkMetrics>,
                "NotAnEvaluator has no matching operator() and must fail the trait");
            static_assert(meta::metric_evaluator_v<ConnectionCountEvaluator, NeuralNetwork, NetworkMetrics>,
                "ConnectionCountEvaluator must fulfill the trait");
        }
    }
}

#endif // EVOAI_EVALUATORS_TEST_HPP
