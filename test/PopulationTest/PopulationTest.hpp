#ifndef EVOAI_POPULATION_TEST_HPP
#define EVOAI_POPULATION_TEST_HPP

#include <gtest/gtest.h>
#include <EvoAI.hpp>

namespace EvoAI{
    namespace Test{
        TEST(PopulationTest, Constructor){
            Population p(150,2,1);
            EXPECT_EQ(6.0,p.getCompatibilityThreshold());
            EXPECT_EQ(120u,p.getMaxAge());
        }
        TEST(PopulationTest, Saving){
            Population p(25,1,2);
            p.setCompatibilityThreshold(2.0);
            p.setMaxAge(250u);
            p.writeToFile("Population.json");
        }
        TEST(PopulationTest, Loading){
            Population p("Population.json");
            EXPECT_EQ(250u,p.getMaxAge());
            EXPECT_EQ(2.0,p.getCompatibilityThreshold());
            EXPECT_EQ(25u,p.getPopulationSize());
            EXPECT_EQ(250u,p.getMaxAge());
        }
    }
}
#endif // EVOAI_POPULATION_TEST_HPP
