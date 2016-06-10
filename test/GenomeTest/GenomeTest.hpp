#ifndef EVOAI_GENOME_TEST_HPP
#define EVOAI_GENOME_TEST_HPP

#include <gtest/gtest.h>
#include <EvoAI.hpp>

namespace EvoAI{
    namespace Test{
        TEST(GenomeTest,ConstructorWithInputs){
            Genome cppn(3,2,true,true);
            EXPECT_EQ(5u,cppn.getNodeChromosomes().size());
            EXPECT_EQ(0u,cppn.getGenomeID());
            EXPECT_EQ(0u,cppn.getSpeciesID());
        }
        TEST(GenomeTest, Mutation){
            /// @todo add tests for some important functions?
        }
    }
}
#endif // EVOAI_GENOME_TEST_HPP