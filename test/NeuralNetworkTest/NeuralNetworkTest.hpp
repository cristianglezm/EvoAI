#ifndef NEURAL_NETWORK_TEST_HPP
#define NEURAL_NETWORK_TEST_HPP

#include <gtest/gtest.h>
#include <EvoAI.hpp>

namespace EvoAI{
    namespace Test{
        class NeuralNetworkTest : public ::testing::Test {
            protected:
                // If the constructor and destructor are not enough for setting up
                // and cleaning up each test, you can define the following methods:
                // Code here will be called immediately after the constructor (right
                // before each test).
                NeuralNetworkTest();
                virtual void SetUp(){
                    
                }
                // Code here will be called immediately after each test (right
                // before the destructor).
                virtual void TearDown(){
                    
                }
                // You can do clean-up work that doesn't throw exceptions here.
                virtual ~NeuralNetworkTest() = default;
            public:
            EvoAI::NeuralNetwork nn1;
            EvoAI::NeuralNetwork nn2;
        };
        NeuralNetworkTest::NeuralNetworkTest() : nn1(1,1,2,1,1.0), nn2(){}
        TEST_F(NeuralNetworkTest,DefaultConstructor){
            EXPECT_EQ(0u, nn2.size());
            nn2.addLayer(EvoAI::NeuronLayer(2,EvoAI::Neuron::Type::HIDDEN, 1.0));
            EvoAI::Neuron n;
            nn2.addNeuron(n,0);
            EXPECT_EQ(1u,nn2.size());
        }
        TEST_F(NeuralNetworkTest,SecondConstructor){
            EXPECT_EQ(3u,nn1.size());
            EXPECT_EQ(2u,nn1[1].size());
            EXPECT_TRUE(nn1.removeNeuron(&nn1[1][0]));
            EXPECT_EQ(1u,nn1[1].size());
        }
        TEST_F(NeuralNetworkTest,JsonConstructor){
            EvoAI::NeuralNetwork nnSave(1,2,1,1,1.0);
            nnSave.writeToFile("testNN.json");
            EvoAI::NeuralNetwork nnJson("testNN.json");
            EXPECT_TRUE(nnJson == nnSave);
        }
    }
}

#endif // NEURAL_NETWORK_TEST_HPP