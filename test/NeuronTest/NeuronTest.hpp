#ifndef EVOAI_NEURON_TEST_HPP
#define EVOAI_NEURON_TEST_HPP

#include <gtest/gtest.h>
#include <EvoAI.hpp>

namespace EvoAI{
    namespace NeuralNetwork{
        namespace Test{
            class NeuronTest : public ::testing::Test {
                protected:
                    // If the constructor and destructor are not enough for setting up
                    // and cleaning up each test, you can define the following methods:
                    // Code here will be called immediately after the constructor (right
                    // before each test).
                    NeuronTest();
                    virtual void SetUp(){
                        n.addConnection(c);
                        n.addConnection(Connection(Link(0,1), Link(1,1),-0.22));
                        n.addConnection(Connection(Link(0,1), Link(1,2),-0.52));
                    }
                    // Code here will be called immediately after each test (right
                    // before the destructor).
                    virtual void TearDown(){
                        
                    }
                    // You can do clean-up work that doesn't throw exceptions here.
                    virtual ~NeuronTest() = default;
                public:
                    Neuron n;
                    Connection c;
            };
            NeuronTest::NeuronTest() : n(Neuron::Type::INPUT), c(Link(0,1), Link(1,0),-0.4){}
            TEST_F(NeuronTest, DefaultContructor){
                EXPECT_TRUE(n.removeConnection(c));
                EXPECT_EQ(Neuron::Type::INPUT, n.getType());
                EXPECT_EQ(0.0, n.getOutput());
                EXPECT_EQ(0.0, n.getSum());
                EXPECT_EQ(1.0, n.addSum(1.0).getSum());
                EXPECT_EQ(1.0, n.setOutput(1.0).getOutput());
                n.setBiasWeight(1.0);
                EXPECT_EQ(1.0, n.getBiasWeight());
            }
            TEST_F(NeuronTest,ClearConnections){
                n.clearConnections();
                EXPECT_EQ(0u, n.getConnections().size());
            }
        }
    }
}

#endif // EVOAI_NEURON_TEST_HPP