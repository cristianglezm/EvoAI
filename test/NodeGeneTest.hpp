#ifndef EVOAI_NODE_GENE_TEST_HPP
#define EVOAI_NODE_GENE_TEST_HPP

#include <limits>

#include <gtest/gtest.h>
#include <EvoAI.hpp>

namespace EvoAI{
    namespace Test{
        TEST(NodeGeneTest,Constructor){
            NodeGene ng(0,1,Neuron::Type::INPUT,Neuron::ActivationType::GAUSSIAN);
            auto innovationID = std::hash<NodeGene>{}(ng);
            EXPECT_EQ(innovationID,ng.getInnovationID());
            EXPECT_EQ(Neuron::ActivationType::GAUSSIAN, ng.getActType());
            EXPECT_EQ(0u,ng.getLayerID());
            EXPECT_EQ(1u,ng.getNeuronID());
            EXPECT_EQ(Neuron::Type::INPUT,ng.getNeuronType());
        }
        TEST(NodeGeneTest, invalidJsonData){
            JsonBox::Value v;
            v["layerID"] = JsonBox::Value("adasd");
            v["neuronID"] = JsonBox::Value("asdad");
            v["biasWeight"] = JsonBox::Value("sadad");
            v["nrnType"] = JsonBox::Value("dsaf");
            v["actType"] = JsonBox::Value("sfs");
            EXPECT_THROW(NodeGene(v.getObject()), std::invalid_argument);
            v["layerID"] = JsonBox::Value("-1");
            v["neuronID"] = JsonBox::Value("-1");
            v["biasWeight"] = JsonBox::Value(9);
            v["nrnType"] = JsonBox::Value("dsaf");
            v["actType"] = JsonBox::Value("sfs");
            auto ng = NodeGene(v.getObject());
            EXPECT_EQ(0u, ng.getInnovationID());
            EXPECT_EQ(Neuron::ActivationType::SIGMOID, ng.getActType());
            EXPECT_EQ(0u, ng.getLayerID());
            EXPECT_EQ(0u, ng.getNeuronID());
            EXPECT_EQ(Neuron::Type::HIDDEN, ng.getNeuronType());
        }
        TEST(NodeGeneTest, Equal){
            NodeGene ng1(0,1,Neuron::Type::INPUT,Neuron::ActivationType::GAUSSIAN);
            NodeGene ng2(0,1,Neuron::Type::INPUT,Neuron::ActivationType::GAUSSIAN);
            EXPECT_EQ(ng1, ng2);
        }
    }
}
#endif // EVOAI_NODE_GENE_TEST_HPP
