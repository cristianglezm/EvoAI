#ifndef EVOAI_NEURONLAYER_TEST_HPP
#define EVOAI_NEURONLAYER_TEST_HPP

#include <gtest/gtest.h>
#include <EvoAI.hpp>

namespace EvoAI{
    namespace Test{
        class NeuronLayerTest : public ::testing::Test {
            protected:
                // If the constructor and destructor are not enough for setting up
                // and cleaning up each test, you can define the following methods:
                // Code here will be called immediately after the constructor (right
                // before each test).
                NeuronLayerTest();
                virtual void SetUp(){
                    nl.setActivationType(Neuron::ActivationType::LEAKY_RELU);
                }
                // Code here will be called immediately after each test (right
                // before the destructor).
                virtual void TearDown(){
                    
                }
                // You can do clean-up work that doesn't throw exceptions here.
                virtual ~NeuronLayerTest() = default;
            public:
            NeuronLayer nl;
            NeuronLayer nld;
        };
        NeuronLayerTest::NeuronLayerTest():nl(3,Neuron::Type::HIDDEN,1.0),nld(){}
        TEST_F(NeuronLayerTest,DefaultConstructor){
            EXPECT_EQ(Neuron::ActivationType::SIGMOID, nld.getActivationType());
            EXPECT_EQ(0.0,nld.getBias());
            EXPECT_EQ(3,nld.getCyclesLimit());
            EXPECT_EQ(Neuron::Type::HIDDEN, nld.getType());
            EXPECT_EQ(0u, nld.size());
            Neuron n(Neuron::Type::HIDDEN);
            nld.addNeuron(n);
            EXPECT_EQ(1u, nld.size());
            EXPECT_TRUE(nld.hasNeuron(&n));
        }
        TEST_F(NeuronLayerTest,SecondConstructor){
            EXPECT_EQ(Neuron::ActivationType::LEAKY_RELU, nl.getActivationType());
            EXPECT_EQ(1.0,nl.getBias());
            EXPECT_EQ(3,nl.getCyclesLimit());
            EXPECT_EQ(Neuron::Type::HIDDEN, nl.getType());
            EXPECT_EQ(3u, nl.size());
            Neuron n(Neuron::Type::HIDDEN);
            nl.addNeuron(n);
            EXPECT_EQ(4u, nl.size());
            EXPECT_TRUE(nl.hasNeuron(&n));
        }
    }
}

#endif // EVOAI_NEURONLAYER_TEST_HPP