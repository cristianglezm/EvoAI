#ifndef EVOAI_NODE_GENE_TEST_HPP
#define EVOAI_NODE_GENE_TEST_HPP

#include <gtest/gtest.h>
#include <EvoAI.hpp>

namespace EvoAI{
    namespace Test{
        TEST(NodeGeneTest,Constructor){
            NodeGene ng(0,1,Neuron::Type::INPUT,Neuron::ActivationType::GAUSSIAN);
            ng.setInnovationID(20);
            EXPECT_EQ(20u,ng.getInnovationID());
            EXPECT_EQ(Neuron::ActivationType::GAUSSIAN, ng.getActType());
            EXPECT_EQ(0u,ng.getLayerID());
            EXPECT_EQ(1u,ng.getNeuronID());
            EXPECT_EQ(Neuron::Type::INPUT,ng.getNeuronType());
        }
        TEST(NodeGeneTest, Equal){
            NodeGene ng1(0,1,Neuron::Type::INPUT,Neuron::ActivationType::GAUSSIAN);
            NodeGene ng2(0,1,Neuron::Type::INPUT,Neuron::ActivationType::GAUSSIAN);
            EXPECT_EQ(ng1, ng2);
        }
    }
}
#endif // EVOAI_NODE_GENE_TEST_HPP