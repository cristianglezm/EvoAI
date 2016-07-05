#ifndef EVOAI_SPECIES_TEST_HPP
#define EVOAI_SPECIES_TEST_HPP

#include <gtest/gtest.h>
#include <EvoAI.hpp>

namespace EvoAI{
    namespace Test{
        TEST(SpeciesTest, Constructor){
            Species sp(1u,true);
            EXPECT_EQ(1u,sp.getID());
            EXPECT_TRUE(sp.isNovel());
            EXPECT_FALSE(sp.isKillable());
            EXPECT_EQ(0u,sp.getAge());
            EXPECT_EQ(0u,sp.getSize());
        }
        TEST(SpeciesTest, saving){
            Species sp(24,true);
            EXPECT_EQ(24u,sp.getID());
            EXPECT_TRUE(sp.isNovel());
            sp.setAge(250);
            sp.writeToFile("Species.json");
        }
        TEST(SpeciesTest, loading){
            Species sp("Species.json");
            EXPECT_EQ(24u,sp.getID());
            EXPECT_EQ(250u,sp.getAge());
            EXPECT_TRUE(sp.isNovel());
            EXPECT_FALSE(sp.isKillable());
        }
    }
}
#endif // EVOAI_SPECIES_TEST_HPP