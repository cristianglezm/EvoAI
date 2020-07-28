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
        }
        TEST(GenomeTest, WritingAndLoading){
            Genome gL(3,2,1,true,true);
            gL.writeToFile("gL.json");
            Genome gL2("gL.json");
            EXPECT_EQ(gL, gL2);
        }
        TEST(GenomeTest, isSorted){
            Genome g(2,5,2, false, true);
            EXPECT_TRUE(std::is_sorted(std::begin(g.getNodeChromosomes()),std::end(g.getNodeChromosomes())));
            EXPECT_TRUE(std::is_sorted(std::begin(g.getConnectionChromosomes()), std::end(g.getConnectionChromosomes())));
        }
        TEST(GenomeTest, getMatchingNodeGenes){
            Genome g1(1,1,1,false,true);
            Genome g2(1,5,1,false,true);
            auto m = Genome::getMatchingNodeGenes(g1,g2);
            EXPECT_EQ(m.first.size(), m.second.size());
            for(auto i=0u;i<m.first.size();++i){
                EXPECT_EQ(m.first[i], m.second[i]);
            }

            Genome g3(1,5,false,true);
            Genome g4(1,1,false,true);
            m = Genome::getMatchingNodeGenes(g3,g4);
            EXPECT_EQ(m.first.size(), m.second.size());
            for(auto i=0u;i<m.first.size();++i){
                EXPECT_EQ(m.first[i], m.second[i]);
            }
        }
        TEST(GenomeTest, getMatchingConnectionGenes){
            Genome g1(1,1,1,false,true);
            Genome g2(1,5,1,false,true);
            auto m = Genome::getMatchingConnectionGenes(g1,g2);
            EXPECT_EQ(m.first.size(), m.second.size());
            for(auto i=0u;i<m.first.size();++i){
                EXPECT_EQ(m.first[i], m.second[i]);
            }

            Genome g3(1,1,1,false,true);
            Genome g4(4,3,1,false,true);
            m = Genome::getMatchingConnectionGenes(g3,g4);
            EXPECT_EQ(m.first.size(), m.second.size());
        }
        TEST(GenomeTest, getDisjointGenes){
            //check no disjoint
            auto hasher1 = std::hash<NodeGene>{};
            auto hasher2 = std::hash<ConnectionGene>{};
            Genome g1(1,1,1,false,true);
            Genome g2(1,1,1,false,true);
            auto noDisjoints = Genome::getDisjointGenes(g1, g2);
            EXPECT_EQ(0u, noDisjoints.first.first.size());
            EXPECT_EQ(0u, noDisjoints.first.second.size());
            EXPECT_EQ(0u, noDisjoints.second.first.size());
            EXPECT_EQ(0u, noDisjoints.second.second.size());

            std::array<std::size_t, 4> dng2 = {hasher1(NodeGene(1,1)), hasher1(NodeGene(1,2)), 
                                               hasher1(NodeGene(1,3)), hasher1(NodeGene(1,4))};
            std::array<std::size_t, 4> dcg2 = {hasher2(ConnectionGene(NodeGene(0,0), NodeGene(1,1), 1)), hasher2(ConnectionGene(NodeGene(0,0), NodeGene(1,2), 1)),
                                               hasher2(ConnectionGene(NodeGene(0,0), NodeGene(1,3), 1)), hasher2(ConnectionGene(NodeGene(0,0), NodeGene(1,4), 1))};
            std::sort(std::begin(dng2), std::end(dng2));
            Genome g3(1,1,1,false,true);
            Genome g4(1,5,1,false,true);
            /*
            XX->{XX, ...} connected to
            [*] = disjoints
            (*) = excess
            nodes
                00, 10,   ,   ,   , [20]
                00, 10, [11, 12, 13, 14], (20)
            connections
                00->{10}                , [10->{20}]
                00->{10, [11, 12, 13, 14]}, (10->{20}, 11->{20}, 12->{20}, 13->{20}, 14->{20})
            */
            auto dg = Genome::getDisjointGenes(g3, g4);
            EXPECT_EQ(dg.first.first[0].getInnovationID(), hasher1(NodeGene(2,0)));
            for(auto i=0u;i<dg.first.second.size(); ++i){
                EXPECT_EQ(dg.first.second[i].getInnovationID(), dng2[i]);
            }
            EXPECT_EQ(dg.second.first[0].getInnovationID(), hasher2(ConnectionGene(NodeGene(1,0), NodeGene(2,0), 1)));
            for(auto i=0u;i<dg.second.second.size(); ++i){
                EXPECT_EQ(dg.second.second[i].getInnovationID(), dcg2[i]);
            }
        }
        TEST(GenomeTest, getExcessGenes){
            //check no excess
            auto hasher1 = std::hash<NodeGene>{};
            auto hasher2 = std::hash<ConnectionGene>{};
            Genome g1(1,1,1,false,true);
            Genome g2(1,1,1,false,true);
            auto noExcess = Genome::getExcessGenes(g1, g2);
            EXPECT_EQ(0u, noExcess.first.first.size());
            EXPECT_EQ(0u, noExcess.first.second.size());
            EXPECT_EQ(0u, noExcess.second.first.size());
            EXPECT_EQ(0u, noExcess.second.second.size());

            Genome g3(1,1,1, false, true);
            Genome g4(1,5,1, false, true);
            auto excess = Genome::getExcessGenes(g3,g4);
            /*
            XX->{XX, ...} connected to
            [*] = disjoints
            (*) = excess
            nodes
                00, 10,   ,   ,   , (20)
                00, 10, [11, 12, 13, 14], (20)
            connections
                00->{10}                , (10->{20})
                00->{10, [11, 12, 13, 14]}, (10->{20}, 11->{20}, 12->{20}, 13->{20}, 14->{20})
            */
            std::array<std::size_t, 1> eng1 = {hasher1(NodeGene(2,0))};
            for(auto i=0u;i<excess.first.first.size();++i){
                EXPECT_EQ(excess.first.first[i].getInnovationID(), eng1[i]);
                EXPECT_EQ(excess.first.second[i].getInnovationID(), eng1[i]);
            }
            std::array<std::size_t, 2> ecg1 = {hasher2(ConnectionGene(NodeGene(1,0), NodeGene(2,0), 1))};
            std::array<std::size_t, 5> ecg2 = {hasher2(ConnectionGene(NodeGene(1,0), NodeGene(2,0), 1)), hasher2(ConnectionGene(NodeGene(1,1), NodeGene(2,0), 1)),
                                                hasher2(ConnectionGene(NodeGene(1,2), NodeGene(2,0), 1)), hasher2(ConnectionGene(NodeGene(1,3), NodeGene(2,0), 1)),
                                                hasher2(ConnectionGene(NodeGene(1,4), NodeGene(2,0), 1))};
            std::sort(std::begin(ecg2), std::end(ecg2));
            for(auto i=0u;i<excess.second.first.size();++i){
                EXPECT_EQ(excess.second.first[i].getInnovationID(), ecg1[i]);
            }
            for(auto i=0u;i<excess.second.second.size();++i){
                EXPECT_EQ(excess.second.second[i].getInnovationID(), ecg2[i]);
            }
            
        }
        TEST(GenomeTest, Mutation){
            Genome g(3,2,true,true);
            EXPECT_EQ(0u,g.getGenomeID());
            EXPECT_EQ(0u,g.getSpeciesID());
            EXPECT_EQ(5u,g.getNodeChromosomes().size());
            EXPECT_EQ(6u,g.getConnectionChromosomes().size());
            g.mutateAddNode();
            EXPECT_EQ(6u,g.getNodeChromosomes().size());
            EXPECT_EQ(8u,g.getConnectionChromosomes().size());
            g.mutateAddConnection();
            EXPECT_EQ(9u,g.getConnectionChromosomes().size());
            g.mutate();
            EXPECT_TRUE(g.isValid());
        }
        TEST(GenomeTest, Reproduce){
            std::vector<std::unique_ptr<Genome>> fathers;
            std::vector<std::unique_ptr<Genome>> mothers;
            std::vector<std::unique_ptr<Genome>> children;
            fathers.reserve(4);
            mothers.reserve(4);
            children.reserve(4);
            for(auto i=0u;i<4;++i){
                fathers.emplace_back(std::make_unique<Genome>(6,3,true,true));
                mothers.emplace_back(std::make_unique<Genome>(6,3,true,true));
            }
            for(auto i=0u;i<4;++i){
                children.emplace_back(Genome::reproduce(*fathers[i],*mothers[i]));
            }
            std::array<std::unique_ptr<Genome>,2> grandkids;
            grandkids[0] = Genome::reproduce(*children[0],*children[1]);
            grandkids[1] = Genome::reproduce(*children[2],*children[3]);

            auto greatGrandkids1 = Genome::reproduce(*grandkids[0],*grandkids[1]);
            auto greatGrandkids2 = Genome::reproduce(*grandkids[0],*grandkids[1]);
            // check that there is no speciation
            EXPECT_TRUE(Genome::distance(*grandkids[0],*grandkids[1]) < 10);
            EXPECT_TRUE(Genome::distance(*greatGrandkids1,*fathers[0]) < 10);
            EXPECT_TRUE(Genome::distance(*greatGrandkids2,*fathers[0]) < 10);
            EXPECT_TRUE(greatGrandkids1->isValid());
        }
        TEST(GenomeTest, Distance){
            Genome g1(1,2,true,true);
            Genome g2(1,2,true,true);
            EXPECT_TRUE(Genome::distance(g1,g2) < 10.0);
        }
    }
}
#endif // EVOAI_GENOME_TEST_HPP
