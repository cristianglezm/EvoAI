#ifndef EVOAI_CONNECTION_GENE_TEST_HPP
#define EVOAI_CONNECTION_GENE_TEST_HPP

#include <gtest/gtest.h>
#include <EvoAI.hpp>

namespace EvoAI{
    namespace Test{
        TEST(ConnectionGeneTest, ConstructorWithArgs){
            ConnectionGene cg(NodeGene(1,2),NodeGene(2,1),0.2);
            EXPECT_TRUE(cg.isEnabled());
            EXPECT_FALSE(cg.isFrozen());
            EXPECT_EQ(0.2,cg.getWeight());
            auto innovationID = std::hash<ConnectionGene>{}(cg);
            EXPECT_EQ(innovationID, cg.getInnovationID());
            EXPECT_EQ(2u,cg.getDest().layer);
            EXPECT_EQ(1u,cg.getDest().neuron);
            ConnectionGene cg1(cg);
            EXPECT_EQ(cg1, cg);
        }
        TEST(ConnectionGeneTest, invalidJsonData){
            {
                JsonBox::Value linkJson;
                linkJson["layer"] = "dasd";
                linkJson["neuron"] = "asdad";
                JsonBox::Value connJson;
                connJson["src"] = linkJson;
                connJson["dest"] = linkJson;
                connJson["weight"] = 9;
                connJson["gradient"] = 2;
                connJson["frozen"] = false;
                JsonBox::Value cgJson;
                cgJson["enabled"] = false;
                cgJson["frozen"] = true;
                cgJson["Connection"] = connJson;
                EXPECT_THROW(ConnectionGene(cgJson.getObject()), std::invalid_argument);
            }
            {
                JsonBox::Value linkJson;
                linkJson["layer"] = "-1";
                linkJson["neuron"] = "-1";
                JsonBox::Value connJson;
                connJson["src"] = linkJson;
                connJson["dest"] = linkJson;
                connJson["weight"] = 9;
                connJson["gradient"] = 2;
                connJson["frozen"] = false;
                JsonBox::Value cgJson;
                cgJson["enabled"] = false;
                cgJson["frozen"] = true;
                cgJson["Connection"] = connJson;
                auto cg = ConnectionGene(cgJson.getObject());
                EXPECT_EQ(0u, cg.getSrc().layer);
                EXPECT_EQ(0u, cg.getSrc().neuron);
                EXPECT_EQ(0u, cg.getDest().layer);
                EXPECT_EQ(0u, cg.getDest().neuron);
                EXPECT_EQ(9.0, cg.getWeight());
                EXPECT_EQ(2.0, cg.getConnection().getGradient());
                EXPECT_EQ(false, cg.getConnection().isFrozen());
                EXPECT_EQ(false, cg.isEnabled());
                EXPECT_EQ(true, cg.isFrozen());
            }
        }
    }
}
#endif // EVOAI_CONNECTION_GENE_TEST_HPP
