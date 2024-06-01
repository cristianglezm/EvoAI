#ifndef EVOAI_ACTIVATIONS_TEST_HPP
#define EVOAI_ACTIVATIONS_TEST_HPP

#include <gtest/gtest.h>
#include <EvoAI/Loss.hpp>
#include <EvoAI.hpp>
#include <EvoAI/DataLoader.hpp>
#include <fstream>

namespace EvoAI{
    namespace Test{
        template<typename Act, typename Der>
        std::pair<double, double> gradientCheckingAct(Act&& act, Der&& der, double sum) noexcept{
            const double epsilon = 1e-8;
            double d1 = (act(sum + epsilon) - act(sum - epsilon)) / (2.0 * epsilon);
            double d2 = (der(sum));
            return std::make_pair(d1, d2);
        }
        template<typename Act, typename Der, typename...Args>
        std::pair<double, double> gradientCheckingAct(Act&& act, Der&& der, double sum, const Args&...args) noexcept{
            const double epsilon = 1e-8;
            double d1 = (act(sum + epsilon, args...) - act(sum - epsilon, args...)) / (2.0 * epsilon);
            double d2 = (der(sum,args...));
            return std::make_pair(d1, d2);
        }
        TEST(ActivationsTests, Identity){
            auto result = gradientCheckingAct(Activations::identity, Derivatives::identity, 42.0);
            std::cout << "[ " << result.first << " == " << result.second << "]" << std::endl;
            EXPECT_NEAR(result.first, result.second, 1e-2);
        }
        TEST(ActivationsTests, Sigmoid){
            auto result = gradientCheckingAct(Activations::sigmoid, Derivatives::sigmoid, 42.0);
            std::cout << "[ " << result.first << " == " << result.second << "]" << std::endl;
            EXPECT_NEAR(result.first, result.second, 1e-2);
        }
        TEST(ActivationsTests, steepenedSigmoid){
            auto result = gradientCheckingAct(Activations::steepenedSigmoid, Derivatives::steepenedSigmoid, 42.0);
            std::cout << "[ " << result.first << " == " << result.second << "]" << std::endl;
            EXPECT_NEAR(result.first, result.second, 1e-2);
        }
        TEST(ActivationsTests, swish){
            auto result = gradientCheckingAct(Activations::swish, Derivatives::swish, 42.0, 1.0);
            std::cout << "[ " << result.first << " == " << result.second << "]" << std::endl;
            EXPECT_NEAR(result.first, result.second, 1e-2);
        }
        TEST(ActivationsTests, tanh){
            auto result = gradientCheckingAct(Activations::tanh, Derivatives::tanh, 42.0);
            std::cout << "[ " << result.first << " == " << result.second << "]" << std::endl;
            EXPECT_NEAR(result.first, result.second, 1e-2);
        }
        TEST(ActivationsTests, sinusoid){
            auto result = gradientCheckingAct(Activations::sinusoid, Derivatives::sinusoid, 42.0);
            std::cout << "[ " << result.first << " == " << result.second << "]" << std::endl;
            EXPECT_NEAR(result.first, result.second, 1e-2);
        }
        TEST(ActivationsTests, cosine){
            auto result = gradientCheckingAct(Activations::cosine, Derivatives::cosine, 42.0);
            std::cout << "[ " << result.first << " == " << result.second << "]" << std::endl;
            EXPECT_NEAR(result.first, result.second, 1e-2);
        }
        TEST(ActivationsTests, tan){
            auto result = gradientCheckingAct(Activations::tan, Derivatives::tan, 42.0);
            std::cout << "[ " << result.first << " == " << result.second << "]" << std::endl;
            EXPECT_NEAR(result.first, result.second, 1e-2);
        }
        TEST(ActivationsTests, relu){
            auto result = gradientCheckingAct(Activations::relu, Derivatives::relu, 42.0);
            std::cout << "[ " << result.first << " == " << result.second << "]" << std::endl;
            EXPECT_NEAR(result.first, result.second, 1e-2);
        }
        TEST(ActivationsTests, noisyRelu){
            auto result = gradientCheckingAct(Activations::noisyRelu, Derivatives::noisyRelu, 42.0);
            std::cout << "[ " << result.first << " == " << result.second << "]" << std::endl;
            EXPECT_NEAR(result.first, result.second, 1e-2);
        }
        TEST(ActivationsTests, leakyRelu){
            auto result = gradientCheckingAct(Activations::leakyRelu, Derivatives::leakyRelu, 42.0);
            std::cout << "[ " << result.first << " == " << result.second << "]" << std::endl;
            EXPECT_NEAR(result.first, result.second, 1e-2);
        }
        TEST(ActivationsTests, exponential){
            auto result = gradientCheckingAct(Activations::exponential, Derivatives::exponential, 42.0);
            std::cout << "[ " << result.first << " == " << result.second << "]" << std::endl;
            EXPECT_NEAR(result.first, result.second, 1e+18);
        }
        TEST(ActivationsTests, softmax){
            auto gradientCheckSoftMax = [](){
                const double epsilon = 1e-8;
                double sum = 42.0;
                EvoAI::NeuronLayer nl{3,EvoAI::Neuron::Type::OUTPUT, 1.0};
                nl[0].setOutput(sum + epsilon);
                nl[1].setOutput(1.0 + epsilon);
                nl[2].setOutput(1.0 + epsilon);
                Activations::softmax(nl);
                auto res1 = nl[0].getOutput();
                nl[0].setOutput(sum - epsilon);
                nl[1].setOutput(1.0 - epsilon);
                nl[2].setOutput(1.0 - epsilon);
                Activations::softmax(nl);
                auto res2 = nl[0].getOutput();
                double d1 = (res2 - res1) / (2.0 * epsilon);
                nl[0].setOutput(sum);
                nl[1].setOutput(1.0);
                nl[2].setOutput(1.0);
                Activations::softmax(nl);
                Derivatives::softmax(nl);
                double d2 = nl[0].getGradient();
                return std::make_pair(d1, d2);
            };
            auto result = gradientCheckSoftMax();
            std::cout << "[ " << result.first << " == " << result.second << "]" << std::endl;
            EXPECT_NEAR(result.first, result.second, 1e-2);
        }
        TEST(ActivationsTests, gaussian){
            auto result = gradientCheckingAct(Activations::gaussian, Derivatives::gaussian, 42.0);
            std::cout << "[ " << result.first << " == " << result.second << "]" << std::endl;
            EXPECT_NEAR(result.first, result.second, 1e-2);
        }
        TEST(ActivationsTests, modulus){
            /// non continous fn
            auto result = gradientCheckingAct(Activations::modulus, Derivatives::modulus, 1.0, 2.0);
            std::cout << "[ " << result.first << " == " << result.second << "]" << std::endl;
            EXPECT_NEAR(result.first, result.second, 1e-2);
        }
        TEST(ActivationsTests, square){
            auto result = gradientCheckingAct(Activations::square, Derivatives::square, 42.0);
            std::cout << "[ " << result.first << " == " << result.second << "]" << std::endl;
            EXPECT_NEAR(result.first, result.second, 1e-2);
        }
        TEST(ActivationsTests, cube){
            auto result = gradientCheckingAct(Activations::cube, Derivatives::cube, 42.0);
            std::cout << "[ " << result.first << " == " << result.second << "]" << std::endl;
            EXPECT_NEAR(result.first, result.second, 1e-2);
        }
        TEST(ActivationsTests, softplus){
            auto result = gradientCheckingAct(Activations::softplus, Derivatives::softplus, 42.0);
            std::cout << "[ " << result.first << " == " << result.second << "]" << std::endl;
            EXPECT_NEAR(result.first, result.second, 1e-2);
        }
        TEST(ActivationsTests, clamp){
            auto result = gradientCheckingAct(Activations::clamp, Derivatives::clamp, 42.0);
            std::cout << "[ " << result.first << " == " << result.second << "]" << std::endl;
            EXPECT_NEAR(result.first, result.second, 1e-2);
        }
        TEST(ActivationsTests, inv){
            auto result = gradientCheckingAct(Activations::inv, Derivatives::inv, 42.0);
            std::cout << "[ " << result.first << " == " << result.second << "]" << std::endl;
            EXPECT_NEAR(result.first, result.second, 1e-2);
        }
        TEST(ActivationsTests, log){
            auto result = gradientCheckingAct(Activations::log, Derivatives::log, 42.0);
            std::cout << "[ " << result.first << " == " << result.second << "]" << std::endl;
            EXPECT_NEAR(result.first, result.second, 1e-2);
        }
        TEST(ActivationsTests, abs){
            auto result = gradientCheckingAct(Activations::abs, Derivatives::abs, 42.0);
            std::cout << "[ " << result.first << " == " << result.second << "]" << std::endl;
            EXPECT_NEAR(result.first, result.second, 1e-2);
        }
        TEST(ActivationsTests, hat){
            auto result = gradientCheckingAct(Activations::hat, Derivatives::hat, 42.0);
            std::cout << "[ " << result.first << " == " << result.second << "]" << std::endl;
            EXPECT_NEAR(result.first, result.second, 1e-2);
        }
    }
}
#endif // EVOAI_UTILS_TEST_HPP
