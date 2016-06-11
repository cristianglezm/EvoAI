#ifndef EVOAI_GENOME_TEST_HPP
#define EVOAI_GENOME_TEST_HPP

#include <gtest/gtest.h>
#include <EvoAI.hpp>
#include <array>
#include <vector>

namespace EvoAI{
    namespace Test{
        TEST(GenomeTest,ConstructorWithInputs){
            Genome cppn(3,2,true,true);
            EXPECT_EQ(5u,cppn.getNodeChromosomes().size());
            EXPECT_EQ(0u,cppn.getGenomeID());
            EXPECT_EQ(0u,cppn.getSpeciesID());
            Genome g1(3,3,true,true);
            Genome g2(3,3,true,true);
            for(auto i=0u;i<50;++i){
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
            auto childNN = Genome::makePhenotype(*child);
            childNN->writeToFile("g1g2Childnn.json");
        }
        TEST(GenomeTest, Mutation){
            /// @todo add tests for some important functions?
        }
        TEST(GenomeTest, Reproduce){
            /// @todo add tests for some important functions?
            std::vector<std::unique_ptr<Genome>> fathers;
            std::vector<std::unique_ptr<Genome>> mothers;
            std::vector<std::unique_ptr<Genome>> children;
            for(auto i=0u;i<4;++i){
                fathers.push_back(std::make_unique<Genome>(3,3,true,true));
                mothers.push_back(std::make_unique<Genome>(3,3,true,true));
                if(doAction(0.2)){
                    fathers.back()->mutate();
                    mothers.back()->mutate();
                }
            }
            for(auto i=0u;i<4;++i){
                children.push_back(Genome::reproduce(*fathers[i],*mothers[i]));
            }
            std::array<std::unique_ptr<Genome>,2> grandSons;
            grandSons[0] = Genome::reproduce(*children[0],*children[1]);
            grandSons[1] = Genome::reproduce(*children[2],*children[3]);
            if(doAction(0.4)){
                grandSons[0]->mutate();
                grandSons[1]->mutate();
            }
            auto greatGrandSon1 = Genome::reproduce(*grandSons[0],*grandSons[1]);
            auto greatGrandSon2 = Genome::reproduce(*grandSons[0],*grandSons[1]);
            if(doAction(0.8)){
                greatGrandSon1->mutate();
            }
            std::cout << "Distance grandSons: " << Genome::distance(*grandSons[0],*grandSons[1]) << std::endl;
            std::cout << "Distance greatGrandSon1 With Granpa: " << Genome::distance(*greatGrandSon1,*fathers[0]) << std::endl;
            std::cout << "Distance greatGrandSon2 With Granpa: " << Genome::distance(*greatGrandSon2,*fathers[0]) << std::endl;
            Genome::makePhenotype(*greatGrandSon1)->writeToFile("greatGrandSon1NN.json");
            Genome::makePhenotype(*greatGrandSon2)->writeToFile("greatGrandSon2NN.json");
        }
        TEST(GenomeTest, Distance){
            /// @todo add tests for some important functions?
        }
    }
}
#endif // EVOAI_GENOME_TEST_HPP