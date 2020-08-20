#ifndef EVOAI_POPULATION_TEST_HPP
#define EVOAI_POPULATION_TEST_HPP

#include <gtest/gtest.h>
#include <stack>
#include <EvoAI.hpp>

namespace EvoAI{
    namespace Test{
        TEST(PopulationTest, Constructor){
            Population<Genome> p(10,2,1);
            EXPECT_EQ(p.getPopulationMaxSize(), p.getPopulationSize());
            EXPECT_EQ(2.0,p.getCompatibilityThreshold());
            EXPECT_EQ(120u,p.getMaxAge());
        }
        TEST(PopulationTest, Saving){
            Population<Genome> p([](){
                return Genome(2u,3u,2u, false, false);
            }, 10);
            p.setCompatibilityThreshold(2.0);
            p.setMaxAge(250u);
            EXPECT_EQ(p.getPopulationMaxSize(), p.getPopulationSize());
            p.writeToFile("Population.json");
        }
        TEST(PopulationTest, Loading){
            Population<Genome> p("Population.json");
            EXPECT_EQ(250u,p.getMaxAge());
            EXPECT_EQ(2.0,p.getCompatibilityThreshold());
            EXPECT_EQ(10u,p.getPopulationSize());
            EXPECT_EQ(p.getPopulationMaxSize(), p.getPopulationSize());
            EXPECT_EQ(250u,p.getMaxAge());
        }
        TEST(PopulationTest, PointerSaving){
            Population<Genome*> p;
            std::vector<Genome> genomes;
            genomes.reserve(20);
            for(auto i=0;i<10;++i){
                p.addMember(&genomes.emplace_back(2u,3u,2u, false, false));
            }
            p.setPopulationMaxSize(20);
            p.setCompatibilityThreshold(2.0);
            p.setMaxAge(250u);
            p.writeToFile("Population.json");
        }
        TEST(PopulationTest, PointerLoading){
            ///compile error as expected, you cannot load data into an observing population.
            //Population<Genome*> p("Population.json");
        }
        TEST(PopulationTest, basic){
            Population<Genome> p;
            p.addMember(Genome(2,1));
            p.addMember(Genome(2,1));
            EXPECT_EQ(2u, p.getPopulationSize());
            EXPECT_EQ(1u, p.getSpeciesSize());
            auto found = p.findMember(1);
            EXPECT_TRUE(found != nullptr);
            p.removeMember(*found);
            EXPECT_EQ(1u, p.getPopulationSize());
            p.setCompatibilityThreshold(0.5);
            p.addMember(Genome(2, 3, 1, false, false));
            EXPECT_EQ(2u, p.getSpeciesSize());
            {
                auto sp = std::make_unique<Species<Genome>>(p.getSpeciesSize(), false);
                sp->setAge(250);
                for(auto i=0;i<5;++i){
                    sp->add(Genome(2,1));
                }
                p.addSpecies(std::move(sp));
            }
            EXPECT_EQ(3u, p.getSpeciesSize());
            auto foundSP = p.findSpecies(1);
            EXPECT_TRUE(foundSP != nullptr);
            if(foundSP){
                p.removeSpecies(*foundSP);
            }
            EXPECT_EQ(2u, p.getSpeciesSize());
            p.orderMembersByFitness();
            p.rankWithinSpecies();
            p.eval([](auto& g){
                auto fit = g.getID() == 0 ? 5:1;
                g.setFitness(fit);
            });
            auto best = p.getBestMember();
            EXPECT_EQ(5, best->getFitness());
            EXPECT_EQ(0u, best->getID());
            EXPECT_EQ(0u, best->getSpeciesID());
            p.removeOldSpecies();
            EXPECT_EQ(1u, p.getSpeciesSize());
            auto avg = p.computeAvgFitness();
            auto result = 5 / p.getPopulationSize();
            EXPECT_EQ(avg, result);
            p.clear();
            EXPECT_EQ(0u, p.getPopulationSize());
            EXPECT_EQ(0u, p.getSpeciesSize());

        }
        TEST(PopulationTest, pointerBasic){
            //Population<T*> acting as an observer
            auto currentID = 0;
            std::vector<Genome> genomes;
            Population<Genome*> p;
            genomes.reserve(15);
            p.setPopulationMaxSize(15);
            auto ptr = &genomes.emplace_back(2,1);
            ptr->setID(currentID++);
            p.addMember(ptr);
            ptr = &genomes.emplace_back(2,1);
            ptr->setID(currentID++);
            p.addMember(ptr);
            EXPECT_EQ(2u, p.getPopulationSize());
            EXPECT_EQ(1u, p.getSpeciesSize());
            auto found = p.findMember(1);
            EXPECT_TRUE(found != nullptr);
            p.removeMember(*found);
            EXPECT_EQ(1u, p.getPopulationSize());
            p.setCompatibilityThreshold(0.5);
            ptr = &genomes.emplace_back(22, 34, 52, false, false);
            p.addMember(ptr);
            EXPECT_EQ(2u, p.getSpeciesSize());
            {
                auto sp = std::make_unique<Species<Genome*>>(2, false);
                sp->setAge(250);
                for(auto i=0;i<5;++i){
                    auto tmp = &genomes.emplace_back(2,1);
                    tmp->setID(currentID++);
                    sp->add(tmp);
                }
                p.addSpecies(std::move(sp));
            }
            EXPECT_EQ(3u, p.getSpeciesSize());
            auto foundSP = p.findSpecies(1);
            EXPECT_TRUE(foundSP != nullptr);
            if(foundSP){
                p.removeSpecies(*foundSP);
            }
            EXPECT_EQ(2u, p.getSpeciesSize());
            p.eval([](auto& g){
                auto fit = g.getID() == 0 ? 5:1;
                g.setFitness(fit);
            });
            p.orderMembersByFitness();
            p.rankWithinSpecies();
            auto best = p.getBestMember();
            EXPECT_EQ(5, best->getFitness());
            EXPECT_EQ(0u, best->getID());
            EXPECT_EQ(0u, best->getSpeciesID());
            p.removeOldSpecies();
            EXPECT_EQ(1u, p.getSpeciesSize());
            auto avg = p.computeAvgFitness();
            auto result = 5 / p.getPopulationSize();
            EXPECT_EQ(avg, result);
            p.clear();
            EXPECT_EQ(0u, p.getPopulationSize());
            EXPECT_EQ(0u, p.getSpeciesSize());
        }
        TEST(PopulationTest, Reproduce){
            auto createMember = []() -> auto{ // handle initial creation of population and regrowPopulation
                auto g = Genome(1,1,false,false);
                for(auto& ng:g.getNodeChromosomes()){
                    ng.setBias(0.0d);
                    if(ng.getNeuronType() == Neuron::OUTPUT){
                        ng.setActType(Neuron::ActivationType::IDENTITY);
                    }
                }
                return g;
            };
            auto calcError = [](auto& input, auto& result){
                return std::pow((input * -1) - std::ceil(result), 2.0d);
            };
            const auto maxPop = 101ull;
            Population<Genome> p(createMember, maxPop);
            std::vector<double> results;
            std::vector<double> inputs = {-10, -9, -8, -7, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
            auto errorSum = 99.9d;
            auto eval = [&](auto& g){ // evaluate the population
                g.mutateWeights(2);
                auto nn = Genome::makePhenotype(g);
                errorSum = 0.0d;
                const auto n = inputs.size();
                for(auto i=0u;i<n;++i){
                    nn.setInputs({inputs[i]});
                    auto out = nn.run();
                    results.emplace_back(out[0]);
                    errorSum += calcError(inputs[i], results[i]);
                    nn.reset();
                }
                g.setFitness(n - errorSum);
                results.clear();
            };
            auto testBestMember = [&](auto& g){
                auto nn = Genome::makePhenotype(g);
                inputs.clear();
                std::cout << "Genome ID: " << g.getID() << "\n";
                auto errorSum = 0.0d;
                for(auto i=0u;i<10;++i){
                    inputs.emplace_back(randomGen.random(-100,100));
                    nn.setInputs({inputs[i]});
                    auto out = nn.run();
                    results.emplace_back(out[0]);
                    errorSum += calcError(inputs[i], results[i]);
                    nn.reset();
                    std::cout << "\tinput: " << inputs[i] << " result: " << std::ceil(results[i]) << "\n";
                }
                auto accuracy = (10 - errorSum) * 10;
                accuracy = std::max(0.0d, accuracy);
                std::cout << "Accuracy: " << accuracy << "%" << std::endl;
                errorSum = 0.0d;
                results.clear();
            };
            auto gen = 0u;
            auto sa = SelectionAlgorithms::Tournament<Genome>{maxPop};
            while(errorSum >= 0.01d){
                p.eval(eval);
                p.reproduce(sa, true);
                EXPECT_EQ(p.getPopulationMaxSize(), p.getPopulationSize());
                p.increaseAgeAndRemoveOldSpecies();       // it can extinct a whole population and regrowPopulationFromElites
                p.regrowPopulationFromElites(sa, false); //  won't be able to get a 0 pop to max pop
                p.regrowPopulation(createMember);       //   so we call regrowPopulation in case the pop gets stuck and is killed off.
                std::cout << "\rGeneration: " << gen << " - AVG Fitness: " << p.computeAvgFitness() << " NumSpecies: " << p.getSpeciesSize() << " Error: " << errorSum << "\t\t";
                std::flush(std::cout);
                p.eval(eval);
                p.reproduce(sa, false);
                EXPECT_EQ(p.getPopulationMaxSize(), p.getPopulationSize());
                ++gen;
            }
            std::cout << "\nbest member--------" << std::endl;
            auto m = p.getBestMember();
            if(m){
                testBestMember(*m);
            }
        }
        TEST(PopulationTest, PointerReproduce){
            std::vector<Genome> storage;
            std::stack<Genome*> killedGenomes;
            const auto maxPop = 101ull;
            storage.reserve(maxPop);
            auto genID = [](){
                static auto id = 0u;
                return id++;
            };
            auto createMember = [&]() -> auto{ // handle initial creation of population and regrowPopulation
                if(killedGenomes.empty()){
                    auto& g = storage.emplace_back(1,1,false,false);
                    g.setID(genID());
                    for(auto& ng:g.getNodeChromosomes()){
                        ng.setBias(0.0d);
                        if(ng.getNeuronType() == Neuron::OUTPUT){
                            ng.setActType(Neuron::ActivationType::IDENTITY);
                        }
                    }
                    return &g;
                }else{
                    auto g = killedGenomes.top();
                    killedGenomes.pop();
                    *g = Genome(1,1,false,false);
                    g->setID(genID());
                    for(auto& ng:g->getNodeChromosomes()){
                        ng.setBias(0.0d);
                        if(ng.getNeuronType() == Neuron::OUTPUT){
                            ng.setActType(Neuron::ActivationType::IDENTITY);
                        }
                    }
                    return g;
                }
            };
            auto calcError = [](auto& input, auto& result){
                return std::pow((input * -1) - std::ceil(result), 2.0d);
            };
            Population<Genome*> p(createMember, maxPop);
            std::vector<double> results;
            std::vector<double> inputs = {-10, -9, -8, -7, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
            auto errorSum = 99.9d;
            auto eval = [&](auto& g){
                g.mutateWeights(2);
                auto nn = Genome::makePhenotype(g);
                errorSum = 0.0d;
                const auto n = inputs.size();
                for(auto i=0u;i<n;++i){
                    nn.setInputs({inputs[i]});
                    auto out = nn.run();
                    results.emplace_back(out[0]);
                    errorSum += calcError(inputs[i], results[i]);
                    nn.reset();
                }
                g.setFitness(n - errorSum);
                results.clear();
            };
            auto testBestMember = [&](auto& g){
                auto nn = Genome::makePhenotype(g);
                inputs.clear();
                std::cout << "Genome ID: " << g.getID() << "\n";
                auto errorSum = 0.0d;
                for(auto i=0u;i<10;++i){
                    inputs.emplace_back(randomGen.random(-100,100));
                    nn.setInputs({inputs[i]});
                    auto out = nn.run();
                    results.emplace_back(out[0]);
                    errorSum += calcError(inputs[i], results[i]);
                    nn.reset();
                    std::cout << "\tinput: " << inputs[i] << " result: " << std::ceil(results[i]) << "\n";
                }
                auto accuracy = (10 - errorSum) * 10;
                accuracy = std::max(0.0d, accuracy);
                std::cout << "Accuracy: " << accuracy << "%" << std::endl;
                errorSum = 0.0d;
                results.clear();
            };
            auto removeMembersFromSpecies = [&](auto& ids){
                for(auto& id:ids){
                    for(auto& m:storage){
                        if(m.getSpeciesID() == id){
                            killedGenomes.push(&m);
                        }
                    }
                }
            };
            auto replace = [&](auto& toReplace, auto& toAdd){
                if(toReplace.empty()){ // handle regrowPopulationFromElites result
                    for(auto& add:toAdd){
                        if(!killedGenomes.empty()){
                            auto g = killedGenomes.top();
                            killedGenomes.pop();
                            *g = std::move(add);
                            g->setID(genID());
                            p.addMember(g);
                        }else{ // killedGenomes is empty but regrowPopulationFromElites has given us some genomes to add.
                            auto& g = storage.emplace_back(std::move(add));
                            g.setID(genID());
                            for(auto& ng:g.getNodeChromosomes()){
                                ng.setBias(0.0d);
                                if(ng.getNeuronType() == Neuron::OUTPUT){
                                    ng.setActType(Neuron::ActivationType::IDENTITY);
                                }
                            }
                            p.addMember(&g);
                        }
                    }
                }else if(toReplace.size() == toAdd.size()){ // handle reproduce result
                    for(auto i=0u;i<toAdd.size(); ++i){
                        p.removeMember(*toReplace[i]);
                        toAdd[i].setID(genID());
                        *toReplace[i] = std::move(toAdd[i]);
                        p.addMember(toReplace[i]);
                    }
                }
            };
            auto sa = SelectionAlgorithms::Tournament<Genome*>{maxPop};
            auto gen = 0u;
            while(errorSum >= 0.001d){
                p.eval(eval);
                auto res = p.reproduce(sa, true);
                auto ids = p.increaseAgeAndRemoveOldSpecies();// it can extinct a whole population and regrowPopulationFromElites
                removeMembersFromSpecies(ids);
                replace(res.first, res.second);
                res = p.regrowPopulationFromElites(sa, true);//  won't be able to get a 0 pop to max pop
                replace(res.first, res.second);
                p.regrowPopulation(createMember);            //   so we call regrowPopulation in case the pop gets stuck and is killed off.
                EXPECT_EQ(p.getPopulationMaxSize(), p.getPopulationSize());
                std::cout << "\rGeneration: " << gen << " - AVG Fitness: " << p.computeAvgFitness() << " NumSpecies: " << p.getSpeciesSize() << " Error: " << errorSum << "\t\t";
                std::flush(std::cout);
                p.eval(eval);
                res = p.reproduce(sa, false);
                replace(res.first, res.second);
                ++gen;
            }
            std::cout << "\nbest member--------" << std::endl;
            auto m = p.getBestMember();
            if(m){
                testBestMember(*m);
            }
        }
    }
}
#endif // EVOAI_POPULATION_TEST_HPP