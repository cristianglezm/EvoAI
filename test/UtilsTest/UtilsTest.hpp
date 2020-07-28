#ifndef EVOAI_UTILS_TEST_HPP
#define EVOAI_UTILS_TEST_HPP

#include <gtest/gtest.h>
#include <EvoAI.hpp>

namespace EvoAI{
    namespace Test{
        TEST(UtilsTest, Math){
            auto norm = normalize<double>(50,0.0,1.0,0,100);
            EXPECT_EQ(0.5, norm);
            auto snorm = stableNormalize<double>(1.0,0,100);
            EXPECT_EQ(0.01, snorm);
            auto dm = distanceManhattan<double>(2,2,6,6);
            EXPECT_EQ(80,dm);
            auto dch = distanceChebyshev<double>(2,2,6,6);
            EXPECT_EQ(-4,dch);
            float de = distanceEuclidean<double>(2,2,6,6);
            EXPECT_EQ(6,std::round(de));
            auto dc = distanceCenter<double>(5,5,10,10);
            EXPECT_EQ(2,dc);
        }
        TEST(UtilsTest, Hashing){
            std::size_t hash1 = 0u;
            hashCombine(hash1, 5);
            hashCombine(hash1, 2);
            std::size_t hash2 = 0u;
            hashCombine(hash2, 5);
            hashCombine(hash2, 2);
            auto hash = joinHashes<int>(0u, 5,2);
            EXPECT_TRUE(hash1 == hash2 && hash == hash1);
        }
        TEST(UtilsTest, Random){
            EXPECT_TRUE(randomGen.random(0.99999));
            EXPECT_FALSE(randomGen.random(0.00001));
            auto res1 = randomGen.random(0,5);
            EXPECT_TRUE(res1 >= 0 && res1 <=5);
            auto res2 = randomGen.random(0.0,5.0);
            EXPECT_TRUE(res2 >= 0.0 && res2 <=5.0);
        }
    }
}
#endif // EVOAI_UTILS_TEST_HPP