#ifndef EVOAI_GENOME_TEST_HPP
#define EVOAI_GENOME_TEST_HPP

#include <gtest/gtest.h>
#include <EvoAI.hpp>

namespace EvoAI{
    namespace Test{
        TEST(GenomeTest,ConstructorWithInputs){
            Genome cppn(3,2,true,true);
            Genome nocppn(3,2,false,true);
            for(auto i=0u;i<5;++i){
                cppn.mutate();
                nocppn.mutate();
            }
            Genome::makePhenotype(cppn)->writeToFile("CPPN.json");
            Genome::makePhenotype(nocppn)->writeToFile("NOCPPN.json");
            std::cout << "Distance: " << Genome::distance(cppn,nocppn) << std::endl;
            auto child = Genome::reproduce(cppn,nocppn);
            child->mutate();
            child->writeToFile("GenomeChild.json");
            std::cout << child->toJson() << std::endl;
            Genome::makePhenotype(*child)->writeToFile("Child.json");
        }
        TEST(GenomeTest, Mutation){
            /// @todo
        }
    }
}
#endif // EVOAI_GENOME_TEST_HPP