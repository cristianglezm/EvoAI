#ifndef EVOAI_EVOVECTOR_TEST_HPP
#define EVOAI_EVOVECTOR_TEST_HPP

#include <gtest/gtest.h>
#include <iomanip>
#include <EvoAI.hpp>

namespace EvoAI::Test{
    TEST(EvoVectorTest, Constructor){
        EvoVector<float> evFloat(4);
        for(float f:evFloat){
            EXPECT_TRUE(std::is_floating_point_v<decltype(f)>);
        }
        EXPECT_EQ(evFloat.getID(), 0u);
        EXPECT_EQ(evFloat.getSpeciesID(), 0u);
        EXPECT_EQ(evFloat.getFitness(), 0.0);
    }
    TEST(EvoVectorTest, ConstructorGeneratorFn){
        EvoVector<int> evInt(4, [](){
            static int i = 0;
            return i++;
        });
        int i=0;
        for(const int& f:evInt){
            EXPECT_TRUE(i++ == f);
        }
        EXPECT_EQ(evInt.getID(), 0u);
        EXPECT_EQ(evInt.getSpeciesID(), 0u);
        EXPECT_EQ(evInt.getFitness(), 0.0);
    }
    TEST(EvoVectorTest, constructorDNA){
        EvoVector<int> ev({1,2,3,4});
        EXPECT_EQ(1, ev[0]);
        EXPECT_EQ(2, ev[1]);
        EXPECT_EQ(3, ev[2]);
        EXPECT_EQ(4, ev[3]);
        EXPECT_EQ(ev.getID(), 0u);
        EXPECT_EQ(ev.getSpeciesID(), 0u);
        EXPECT_EQ(ev.getFitness(), 0.0);
    }
    TEST(EvoVectorTest, Saving){
        EvoVector<float> evFloat(3);
        evFloat.toJson().writeToFile("testsData/testEvoVectorFloat.json");
        EvoVector<double> evDouble(3);
        evDouble.toJson().writeToFile("testsData/testEvoVectorDouble.json");
        EvoVector<int> evInt(3);
        evInt.toJson().writeToFile("testsData/testEvoVectorInt.json");
        EvoVector<char> evChar(3);
        evChar.toJson().writeToFile("testsData/testEvoVectorChar.json");
    }
    TEST(EvoVectorTest, Loading){
        {
            JsonBox::Value v;
            v.loadFromFile("testsData/testEvoVectorFloat.json");
            EvoVector<float> evFloat(v.getObject());
            for(auto i=0u;i<evFloat.size();++i){
                EXPECT_EQ(v["dna"].getArray()[i].getFloat(), evFloat[i]);
            }
        }
        {
            JsonBox::Value v;
            v.loadFromFile("testsData/testEvoVectorDouble.json");
            EvoVector<double> evDouble(v.getObject());
            for(auto i=0u;i<evDouble.size();++i){
                EXPECT_EQ(v["dna"].getArray()[i].getDouble(), evDouble[i]);
            }
        }
        {
            JsonBox::Value v;
            v.loadFromFile("testsData/testEvoVectorInt.json");
            EvoVector<int> evInt(v.getObject());
            for(auto i=0u;i<evInt.size();++i){
                EXPECT_EQ(v["dna"].getArray()[i].getInteger(), evInt[i]);
            }
        }
        {
            JsonBox::Value v;
            v.loadFromFile("testsData/testEvoVectorChar.json");
            EvoVector<char> evChar(v.getObject());
            for(auto i=0u;i<evChar.size();++i){
                EXPECT_EQ(v["dna"].getArray()[i].getInteger(), evChar[i]);
            }
        }
    }
    TEST(EvoVectorTest, EvolveWord){
        std::string word("hello world!");
        std::cout << "word is " << std::quoted(word) << std::endl;
        constexpr auto numPop = 500u;
        // create a Population of EvoString, 
        // the first arg is the max population, 
        // the next three are the coefficients and the last is the EvoString argument
        EvoAI::Population<EvoAI::EvoString> pop(numPop,2.0,2.0,1.0, word.size());
        auto correct = 0u;
        auto eval = [&word, &correct](auto& w){
            correct = 0u;
            w.mutate();
            for(auto i=0u;i<word.size();++i){
                if(word[i] == w[i]){
                    correct += 1;
                }
            }
            w.setFitness(correct);
        };
        do{
            pop.eval(eval);
            if(correct != word.size()){
                pop.reproduce(EvoAI::SelectionAlgorithms::Tournament<EvoAI::EvoString>{numPop}, true);
                pop.increaseAgeAndRemoveOldSpecies();
                pop.regrowPopulation(2.0, 2.0, 1.0, word.size());
            }
        }while(correct != word.size());
        auto best = pop.getBestMember();
        if(best){
            std::cout << "The best EvoString values are \"";
            for(auto i=0u;i<best->size();++i){
                std::cout << (*best)[i];
            }
            std::cout << "\"" << '\0' << std::endl;
        }
    }
}

#endif //  EVOAI_EVOVECTOR_TEST_HPP
