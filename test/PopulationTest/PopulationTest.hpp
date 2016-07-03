#ifndef EVOAI_POPULATION_TEST_HPP
#define EVOAI_POPULATION_TEST_HPP

#include <gtest/gtest.h>
#include <EvoAI.hpp>

namespace EvoAI{
    namespace Test{
        TEST(PopulationTest, Constructor){
            /// @todo
            Population p(1500,2,1);
            std::vector<double> x = {0.0,0.0,1.0,1.0};
            std::vector<double> y = {0.0,1.0,0.0,1.0};
            std::vector<double> truth = {0.0,1.0,1.0,0.0};
            auto errorSum = 999.0;
            auto minError = 0.1;
            while(errorSum >= minError){
                std::cout << "Pop size: " << p.getGenomes().size() << std::endl;
                p.orderGenomesByFitness();
                for(auto& g:p.getGenomes()){
                    std::vector<double> results;
                    std::cout << "--start----" << std::endl;
                    g->mutate();
                    std::cout << "Genome ID: " << g->getGenomeID() << std::endl;
                    auto phenotype = Genome::makePhenotype(*g);
                    for(auto i=0u;i<4;++i){
                        phenotype->setInputs({x[i],y[i]});
                        auto out = phenotype->run();
                        results.push_back(out[0]);
                        phenotype->reset();
                        std::cout << "Result: " << out[0] << " input: " << x[i] <<", " << y[i] << " solution: "<< truth[i] << std::endl;
                    }
                    std::cout << "--end----" << std::endl;
                    errorSum = (std::fabs(results[0]) + std::fabs(1.0-results[1]) + std::fabs(1.0-results[2]) + std::fabs(results[3]));
                    g->setFitness(std::pow((2.0 - errorSum), 2));
                    std::cout << "error: "<< errorSum << std::endl;
                    std::cout << "Fitness: " << g->getFitness() << std::endl;
                }
                if(errorSum >= minError){
                    p.reproduce(false,Population::SelectionType::TOURNAMENT);
                }
            }
            std::cout << "Selecting Winner..." << std::endl;
            auto g = p.getBestGenome();
            if(g){
                g->writeToFile("genomeSolvedXOR.json");
                std::cout << "Winner Fitness: " << g->getFitness() << std::endl;
                auto nn = Genome::makePhenotype(*g);
                for(auto i=0u;i<4;++i){
                    nn->setInputs({x[i],y[i]});
                    auto out = nn->run();
                    std::cout << "x: " << x[i] << ", y: " << y[i] << " -> " << out[0] << " == " << truth[i]<< std::endl;
                }
            }
        }
        TEST(PopulationTest, SavingAndLoading){
            Population p(50,1,2);
            p.writeToFile("Population.json");
            Population s("Population.json");
        }
    }
}
#endif // EVOAI_POPULATION_TEST_HPP
