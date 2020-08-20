#ifndef EVOAI_SPECIES_TEST_HPP
#define EVOAI_SPECIES_TEST_HPP

#include <gtest/gtest.h>
#include <EvoAI.hpp>

namespace EvoAI{
    namespace Test{
        TEST(SpeciesTest, DefaultConstructor){
            Species<Genome> sp;
            EXPECT_EQ(0u,sp.getID());
            EXPECT_TRUE(sp.isNovel());
            EXPECT_TRUE(sp.isKillable());
            EXPECT_EQ(0u,sp.getAge());
            EXPECT_EQ(0u,sp.getSize());
        }
        TEST(SpeciesTest, ConstructorIDandNovel){
            Species<Genome> sp(1u,true);
            EXPECT_EQ(1u,sp.getID());
            EXPECT_TRUE(sp.isNovel());
            EXPECT_TRUE(sp.isKillable());
            EXPECT_EQ(0u,sp.getAge());
            EXPECT_EQ(0u,sp.getSize());
        }
        TEST(SpeciesTest, saving){
            Species<Genome> sp(24,true);
            EXPECT_EQ(24u,sp.getID());
            EXPECT_TRUE(sp.isNovel());
            sp.setAge(250);
            for(auto i=0u;i<2u;++i){
                auto g = Genome(1,1,1,false, false);
                g.setFitness(5);
                g.setID(i);
                sp.add(std::move(g));
            }
            sp.writeToFile("Species.json");
        }
        TEST(SpeciesTest, loading){
            Species<Genome> sp("Species.json");
            EXPECT_EQ(24u,sp.getID());
            EXPECT_EQ(250u,sp.getAge());
            EXPECT_TRUE(sp.isNovel());
            EXPECT_TRUE(sp.isKillable());
            EXPECT_EQ(2u,sp.getSize());
        }
        TEST(SpeciesTest, PointerSaving){
            Species<Genome*> sp(24,true);
            std::vector<Genome> genomes;
            genomes.reserve(15);
            for(auto i=0u;i<5u;++i){
                auto g = Genome(1,1,1,false, false);
                g.setFitness(5);
                g.setID(i);
                sp.add(&genomes.emplace_back(g));
            }
            EXPECT_EQ(24u,sp.getID());
            EXPECT_TRUE(sp.isNovel());
            sp.setAge(250);
            EXPECT_EQ(5u,sp.getSize());
            sp.writeToFile("Species.json");
        }
        TEST(SpeciesTest, PointerLoading){
            ///compile error as expected, you can't load data into Species<T*> that act as observers.
            //Species<Genome*> sp("Species.json");
        }
        TEST(SpeciesTest, MemberFunctions){
            Species<Genome> sp(1u, true);
            for(auto i=0u;i<9u;++i){
                auto g = Genome(1,1,1,false, false);
                g.setFitness(5);
                g.setID(i);
                sp.add(std::move(g));
            }
            auto g = Genome(1,1,1,false, false);
            g.setFitness(55.0);
            g.setID(10);
            sp.add(std::move(g));
            EXPECT_EQ(10u, sp.getSize());
            sp.computeAvgFitness();
            sp.computeMaxFitness();
            auto avgFit = sp.getAvgFitness();
            EXPECT_EQ(10.0, avgFit);
            EXPECT_EQ(55.0, sp.getMaxFitness());
            EXPECT_TRUE(sp.isStagnant());
            EXPECT_TRUE(sp.has(6u));
            EXPECT_FALSE(sp.has(69384839u));
            sp.rank();
            auto rep = sp.getRepresentative();
            EXPECT_EQ(55.0, rep->getFitness());
            sp.adjustFitness();
            EXPECT_EQ(5.5, rep->getFitness());
            sp.remove(sp.getChampion()->getID());
            EXPECT_EQ(9u, sp.getSize());
        }
        TEST(SpeciesTest, PointerMemberFunctions){
            Species<Genome*> sp(1u, true);
            std::vector<Genome> genomes;
            genomes.reserve(15);
            for(auto i=0u;i<9u;++i){
                auto g = Genome(1,1,1,false, false);
                g.setFitness(5);
                g.setID(i);
                sp.add(&genomes.emplace_back(g));
            }
            auto g = Genome(1,1,1,false, false);
            g.setFitness(55.0);
            g.setID(10);
            sp.add(&genomes.emplace_back(g));
            EXPECT_EQ(10u, sp.getSize());
            sp.computeAvgFitness();
            sp.computeMaxFitness();
            auto avgFit = sp.getAvgFitness();
            EXPECT_EQ(10.0, avgFit);
            EXPECT_EQ(55.0, sp.getMaxFitness());
            EXPECT_TRUE(sp.isStagnant());
            EXPECT_TRUE(sp.has(&g));
            auto haveNot = Genome(1,1,1,false,false);
            haveNot.setID(69384u);
            EXPECT_FALSE(sp.has(&haveNot));
            sp.rank();
            auto rep = sp.getRepresentative();
            EXPECT_EQ(55.0, rep->getFitness());
            sp.adjustFitness();
            EXPECT_EQ(5.5, rep->getFitness());
            sp.remove(sp.getChampion());
            EXPECT_EQ(9u, sp.getSize());
        }
    }
}
#endif // EVOAI_SPECIES_TEST_HPP