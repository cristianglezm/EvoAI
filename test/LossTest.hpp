#ifndef EVOAI_LOSS_TEST_HPP
#define EVOAI_LOSS_TEST_HPP

#include <gtest/gtest.h>
#include <EvoAI/Loss.hpp>
#include <EvoAI.hpp>
#include <fstream>

namespace EvoAI{
    template<typename Fn>
    std::pair<double, double> gradientChecking() noexcept{
        const double epsilon = 1e-15;
        double expectedOutput = 1.0;
        double output = 0.5;
        Loss::Loss<Fn> fn{Fn{}};
        auto d1 = (fn({expectedOutput}, {output + epsilon}) - fn({expectedOutput}, {output - epsilon})) / (2.0 * epsilon);
        auto d2 = fn.backward({expectedOutput}, {output});
        return std::make_pair(d1, d2[0]);
    }
    namespace Test{
        TEST(LossTest, TypeCheck){
            // uncomment to test, should not compile.
            //struct notALoss{};
            //Loss::Loss<notALoss> s{notALoss{}};
        }
        TEST(LossTest, MeanError){
            auto result = gradientChecking<Loss::MeanError>();
            std::cout << "[ " << result.first << " == " << result.second << "]" << std::endl;
            EXPECT_NEAR(result.first, result.second, 1e-2);
        }
        TEST(LossTest, MeanSquaredError){
            auto result = gradientChecking<Loss::MeanSquaredError>();
            std::cout << "[ " << result.first << " == " << result.second << "]" << std::endl;
            EXPECT_NEAR(result.first, result.second, 1e-2);
        }
        TEST(LossTest, MeanAbsoluteError){
            auto result = gradientChecking<Loss::MeanAbsoluteError>();
            std::cout << "[ " << result.first << " == " << result.second << "]" << std::endl;
            EXPECT_NEAR(result.first, result.second, 1e-2);
        }
        TEST(LossTest, BinaryCrossEntropy){
            auto result = gradientChecking<Loss::BinaryCrossEntropy>();
            std::cout << "[ " << result.first << " == " << result.second << "]" << std::endl;
            EXPECT_NEAR(result.first, result.second, 1e-2);
        }
        TEST(LossTest, MultiClassCrossEntropy){
            auto result = gradientChecking<Loss::MultiClassCrossEntropy>();
            std::cout << "[ " << result.first << " == " << result.second << "]" << std::endl;
            EXPECT_NEAR(result.first, result.second, 1e-2);
        }
    }
}

#endif // EVOAI_LOSS_TEST_HPP