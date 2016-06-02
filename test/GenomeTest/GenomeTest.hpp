#ifndef EVOAI_GENOME_TEST_HPP
#define EVOAI_GENOME_TEST_HPP

#include <gtest/gtest.h>
#include <EvoAI.hpp>

namespace EvoAI{
    namespace Test{
        TEST(GenomeTest,ConstructorWithInputs){
            Genome cppn(3,5,true,true);
             /// @todo
        }
        TEST(GenomeTest, Mutation){
            /// @todo
        }
    }
}
#endif // EVOAI_GENOME_TEST_HPP