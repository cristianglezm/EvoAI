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
            Genome g1(3,3,true,true);
            Genome g2(3,3,true,true);
            for(auto i=0u;i<5;++i){
                g1.mutate();
                g2.mutate();
            }
            std::cout << "Distance: " << Genome::distance(g1,g2) << std::endl;
            g1.writeToFile("g1.json");
            g2.writeToFile("g2.json");
            Genome::makePhenotype(g1)->writeToFile("g1nn.json");
            Genome::makePhenotype(g2)->writeToFile("g2nn.json");
            auto child = Genome::reproduce(g1,g2);
            child->writeToFile("g1g2Child.json");
            Genome::makePhenotype(*child)->writeToFile("g1g2Childnn.json");
        }
        TEST(GenomeTest, Mutation){
            /// @todo add tests for some important functions?
        }
    }
}
#endif // EVOAI_GENOME_TEST_HPP