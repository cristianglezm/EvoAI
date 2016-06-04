#ifndef EVOAI_CONNECTION_GENE_TEST_HPP
#define EVOAI_CONNECTION_GENE_TEST_HPP

#include <gtest/gtest.h>
#include <EvoAI.hpp>

namespace EvoAI{
    namespace Test{
        TEST(ConnectionGene,ConstructorWithArgs){
            ConnectionGene cg(NodeGene(1,2),NodeGene(2,1),0.2);
            EXPECT_TRUE(cg.isEnabled());
            EXPECT_FALSE(cg.isFrozen());
            EXPECT_EQ(0.2,cg.getWeight());
            cg.setInnovationID(20u);
            EXPECT_EQ(20u,cg.getInnovationID());
            EXPECT_EQ(2u,cg.getDest().layer);
            EXPECT_EQ(1u,cg.getDest().neuron);
        }
    }
}
#endif // EVOAI_CONNECTION_GENE_TEST_HPP