#ifndef NEURON_TEST_HPP
#define NEURON_TEST_HPP

#include <gtest/gtest.h>
#include <EvoAI.hpp>

namespace EvoAI{
    namespace Test{
        class NeuronTest : public ::testing::Test {
            protected:
            // You can do set-up work for each test here.
            NeuronTest(){
                
            }
            // You can do clean-up work that doesn't throw exceptions here.
            virtual ~NeuronTest(){
                
            }
            // If the constructor and destructor are not enough for setting up
            // and cleaning up each test, you can define the following methods:
            // Code here will be called immediately after the constructor (right
            // before each test).
            virtual void SetUp(){
                
            }
            // Code here will be called immediately after each test (right
            // before the destructor).
            virtual void TearDown(){
                
            }
        };


        TEST(NeuronTest, buildAndStats){
            using EvoAI::Neuron;
            using EvoAI::Connection;
            Neuron n(Neuron::Type::INPUT);
            Connection c({0,1},{1,0},-0.4);
            n.addConnection(c);
            n.addConnection(Connection({0,1},{1,1},-0.22));
            n.addConnection(Connection({0,1},{1,2},-0.52));
            std::cout << n.toJson() << std::endl;
            EXPECT_TRUE(n.removeConnection(c));
            EXPECT_EQ(Neuron::Type::INPUT, n.getType());
            EXPECT_EQ(0.0, n.getValue());
            EXPECT_EQ(0.0, n.getOldValue());
        }
    }
}

#endif // NEURON_TEST_HPP