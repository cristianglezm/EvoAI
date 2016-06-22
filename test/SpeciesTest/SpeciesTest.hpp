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
    }
}
#endif // EVOAI_SPECIES_TEST_HPP