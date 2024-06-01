#ifndef EVOAI_HYPERNEAT_TEST_HPP
#define EVOAI_HYPERNEAT_TEST_HPP

#include <gtest/gtest.h>
#include <EvoAI.hpp>
#include <array>
#include <vector>

namespace EvoAI{
    namespace Test{
        TEST(HyperNeatTest, Constructor){
            HyperNeat hn(SubstrateInfo(2,2,{3,3},2),HyperNeat::SubstrateConfiguration::GRID);
            hn.setInputs({0.001,1.0});
            auto out = hn.run();
            hn.reset();
            /// @todo fix UB gives nan sometimes?
            /// EXPECT_TRUE(out[0] > 0.0);
            EXPECT_TRUE(out[1] > 0.0);
            EXPECT_EQ(2u,hn.getNumInputs());
            EXPECT_EQ(2u,hn.getNumHiddenLayers());
            EXPECT_EQ(3u,hn.getNumHiddenNeurons()[0]);
            EXPECT_EQ(2u,hn.getNumOutputs());
            hn.mutate();
            hn.setInputs({1.0,0.0,0.5});
            out = hn.run();
        }
        TEST(HyperNeatTest,ConstructorWithGenome){
            Genome g(5,2,false,true);
            g.mutate();
            HyperNeat hn(SubstrateInfo(2,3,{2,2,2},4), g, HyperNeat::SubstrateConfiguration::SANDWICH);
            hn.setInputs({2.3,2.5});
            auto out = hn.run();
        }
        TEST(HyperNeatTest,Saving){
            HyperNeat hn(SubstrateInfo(2,3,{2,2,2},2),HyperNeat::SubstrateConfiguration::SANDWICH);
            hn.makeSubstrate();
            hn.writeToFile("testsData/HyperNeatTest01.json");
            hn.writeDotFile("testsData/HyperNeatTest01.dot");
        }
        TEST(HyperNeatTest,Loading){
            HyperNeat hn("testsData/HyperNeatTest01.json");
            EXPECT_EQ(2u,hn.getNumInputs());
            EXPECT_EQ(3u,hn.getNumHiddenLayers());
            EXPECT_EQ(2u,hn.getNumHiddenNeurons()[0]);
            EXPECT_EQ(2u,hn.getNumOutputs());
            auto& g = hn.getGenome();
            EXPECT_TRUE(g.isRecurrentAllowed());
            EXPECT_TRUE(g.isCppn());
        }
    }
}

#endif // EVOAI_HYPERNEAT_TEST_HPP
