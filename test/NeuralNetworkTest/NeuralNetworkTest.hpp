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
        NeuralNetworkTest::NeuralNetworkTest() : nn1(1,1,5,1,1.0), nn2(){}
        TEST_F(NeuralNetworkTest,DefaultConstructor){
            //layers size
            EXPECT_EQ(0u, nn2.size());
            nn2.addLayer(EvoAI::NeuronLayer(2,EvoAI::Neuron::Type::HIDDEN, 1.0));
            EvoAI::Neuron n;
            nn2.addNeuron(n,0);
            //layers size
            EXPECT_EQ(1u,nn2.size());
        }
        TEST_F(NeuralNetworkTest,SecondConstructor){
            //layer size
            EXPECT_EQ(3u,nn1.size());
            //layer 1 neurons size
            EXPECT_EQ(5u,nn1[1].size());
            //testing getIndex
            auto link = nn1.getIndex(&nn1[1][0]);
            EXPECT_EQ(1u,link.layer);
            EXPECT_EQ(0u,link.neuron);
            // testing removing neuron
            EXPECT_TRUE(nn1.removeNeuron(&nn1[1][0]));
            EXPECT_EQ(4u,nn1[1].size());
        }
        TEST_F(NeuralNetworkTest,RemoveNeuronWithConnections){
            auto nn = EvoAI::createFeedForwardNN(2,1,3,2,1.0);
            auto& nnSelf = *nn;
            nn->writeToFile("notRemovedNeurons.json");
            EXPECT_TRUE(nn->removeNeuron(&nnSelf[1][1]));
            nn->writeToFile("removedNeurons.json");
        }
        TEST_F(NeuralNetworkTest,getNeurons){
            auto nn = EvoAI::createFeedForwardNN(2,1,3,2,1.0);
            for(auto n:nn->getNeurons()){
                EXPECT_TRUE(n!=nullptr);
            }
            EXPECT_EQ(7u,nn->getNeurons().size());
            nn->addNeuron(Neuron(),1);
            EXPECT_EQ(8u,nn->getNeurons().size());
            EXPECT_TRUE(nn->removeNeuron(&(*nn)[1][1]));
            EXPECT_EQ(7u,nn->getNeurons().size());
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
