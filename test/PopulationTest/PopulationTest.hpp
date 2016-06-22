#ifndef EVOAI_POPULATION_TEST_HPP
#define EVOAI_POPULATION_TEST_HPP

#include <gtest/gtest.h>
#include <EvoAI.hpp>

namespace EvoAI{
    namespace Test{
        TEST(PopulationTest, Constructor){
            /// @todo
            Population p(15,2,1,1,false,false);
            std::vector<double> x = {0.0,0.0,1.0,1.0};
            std::vector<double> y = {0.0,1.0,0.0,1.0};
            std::vector<double> truth = {0.0,1.0,1.0,0.0};
            auto errorSum = 999.0;
            while(errorSum > 0.1){
                for(auto& g:p.getGenomes()){
                    std::vector<double> results;
                    std::cout << "--start----" << std::endl;
                    for(auto i=0u;i<4;++i){
                        auto phenotype = Genome::makePhenotype(*g);
                        phenotype->setInputs({x[i],y[i]});
                        auto out = phenotype->run();
                        results.push_back(out[0]);
                        std::cout << "Result: " << out[0] << " input: " << x[i] <<", " << y[i] << " solution: "<< truth[i] << std::endl;
                    }
                    std::cout << "--end----" << std::endl;
                    errorSum = (std::fabs(results[0]) + std::fabs(1.0-results[1])
                                + std::fabs(1.0-results[2]) + std::fabs(results[3]));
                    g->setFitness(std::pow((4.0 - errorSum), 2));
                    g->mutate(0.01,0.02,0.001,0.8,0.5,0.0);
                    std::cout << "error: "<< errorSum << std::endl;
                    std::cout << "Fitness: " << g->getFitness() << std::endl;
                }
                //p.reproduce(true);
            }
            std::cout << "Selecting Winner..." << std::endl;
            auto g = p.getBestGenome();
            if(g){
                g->writeToFile("genomeSolvedXOR.json");
                auto nn = Genome::makePhenotype(*g);
                for(auto i=0u;i<4;++i){
                    nn->setInputs({x[i],y[i]});
                    auto out = nn->run();
                    std::cout << "x: " << x[i] << ", y: " << y[i] << " -> " << (out[0] > 0.5 ? 1:0) << " == " << truth[i]<< std::endl;
                }
            }
        }
    }
}
#endif // EVOAI_POPULATION_TEST_HPP