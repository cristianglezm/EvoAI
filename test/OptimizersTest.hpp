#ifndef EVOAI_OPTIMIZERS_TEST_HPP
#define EVOAI_OPTIMIZERS_TEST_HPP

#include <limits>

#include <gtest/gtest.h>
#include <EvoAI.hpp>

namespace EvoAI{
    namespace Test{
        /**
         * @brief make a neural network with gradients to adjust weights to do: f(x) = (x * (-1)) * 2
         */
        std::unique_ptr<NeuralNetwork> makeNeuralNetworkWithGradients() noexcept{
            auto nn = EvoAI::createFeedForwardNN(1,1,{1},1,0.0);
            (*nn)[1].setActivationType(EvoAI::Neuron::ActivationType::IDENTITY);
            (*nn)[2].setActivationType(EvoAI::Neuron::ActivationType::IDENTITY);

            (*nn)[0][0][0].setWeight(1.0);
            (*nn)[0][0][0].setGradient(2.0);
            (*nn)[0][0].setBiasWeight(0.0);

            (*nn)[1][0][0].setWeight(1.0);
            (*nn)[1][0][0].setGradient(-1.0);
            (*nn)[1][0].setBiasWeight(0.0);
            
            (*nn)[2][0].setBiasWeight(0.0);
            return nn;
        }
        TEST(OptimizersTest, OptimizerWithSGD){
            auto nn = makeNeuralNetworkWithGradients();
            auto out = nn->forward({30.0});
            std::cout << "before optim output: " << out[0] << std::endl; // show 30
            EXPECT_EQ(static_cast<int>(out[0]), 30);
            Optimizer<SGD, ConstantLR> optim(1.0, 1, SGD(nn->getParameters(),1e-9,true,false), Scheduler<ConstantLR>(ConstantLR()));
            optim.step(0);
            nn->reset();
            out = nn->forward({30.0});
            nn->reset();
            std::cout << "after optim output: " << out[0] << std::endl; // show -60
            EXPECT_EQ(static_cast<int>(out[0]), -60);
            Optimizer<SGD, ConstantLR> op2(optim.toJson().getObject(), nn->getParameters());
            EXPECT_EQ(op2.toJson(), optim.toJson());
        }
        TEST(OptimizersTest, OptimizerWithAdam){
            auto nn = makeNeuralNetworkWithGradients();
            auto out = nn->forward({30.0});
            nn->reset();
            std::cout << "before optim output: " << out[0] << std::endl; // show 30
            EXPECT_EQ(static_cast<int>(out[0]), 30);
            Optimizer optim(1.0, 1, Adam(nn->getParameters(), false, 0.928, 0.999), Scheduler(MultiStepLR({1}, 0.1)));
            std::vector<double> w1 = {200.0, 0.0, 0.0};
            std::vector<double> w2 = {-300.0, 0.5, 0.001};
            for(auto i=0;i<3;++i){
                (*nn)[0][0][0].setGradient(w1[i]);
                (*nn)[1][0][0].setGradient(w2[i]);
                optim.step(i);
            }
            out = nn->forward({30.0});
            nn->reset();
            std::cout << "after optim output: " << out[0] << std::endl; // show -60
            EXPECT_EQ(static_cast<int>(out[0]), -60);
            Optimizer<Adam, MultiStepLR> op2(optim.toJson().getObject(), nn->getParameters());
            EXPECT_EQ(op2.toJson(), optim.toJson());
        }
    }
}
#endif // EVOAI_OPTIMIZERS_TEST_HPP
