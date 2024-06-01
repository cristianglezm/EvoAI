#ifndef EVOAI_UTILS_TEST_HPP
#define EVOAI_UTILS_TEST_HPP

#include <gtest/gtest.h>
#include <EvoAI.hpp>

namespace EvoAI{
    namespace Test{
        TEST(UtilsTest, Math){
            std::cout << "testing scaler..." << std::endl;
            Scaler<double> sc{0.0, 1.0};
            std::vector<double> vec = {0.0, 100.0, 25.0, 50.0, 100.0};
            std::vector<double> truth = {0.0, 100.0, 25.0, 50.0, 100.0};
            std::vector<double> truth_scaled = {0.0, 1.0, 0.25, 0.50, 1.0};
            sc.transform(vec);
            for(auto i=0u;i<vec.size();++i){
                EXPECT_EQ(vec[i], truth_scaled[i]);
            }
            sc.inverseTransform(vec);
            for(auto i=0u;i<vec.size();++i){
                EXPECT_EQ(vec[i], truth[i]);
            }
            std::vector<double*> vecPtr;
            for(auto& v:vec){
                vecPtr.emplace_back(&v);
            }
            sc.transform(vecPtr);
            for(auto i=0u;i<vecPtr.size();++i){
                EXPECT_EQ(*vecPtr[i], truth_scaled[i]);
            }
            sc.inverseTransform(vecPtr.end() - 1, vecPtr.end());
            EXPECT_EQ(*vecPtr.back(), truth.back());
            auto v = sc.toJson();
            v.writeToFile("testsData/scaler.json");
            v.loadFromFile("testsData/scaler.json");
            sc = Scaler<double>(v.getObject());
            EXPECT_EQ(sc.normMin, 0.0);
            EXPECT_EQ(sc.normMax, 1.0);
            EXPECT_EQ(sc.min, 0.0);
            EXPECT_EQ(sc.max, 100.0);
            auto scTest1 = Scaler<float>(v.getObject());
            EXPECT_EQ(scTest1.normMin, 0.0);
            EXPECT_EQ(scTest1.normMax, 1.0);
            EXPECT_EQ(scTest1.min, 0.0);
            EXPECT_EQ(scTest1.max, 100.0);
            auto scTest2 = Scaler<int>(v.getObject());
            EXPECT_EQ(scTest2.normMin, 0.0);
            EXPECT_EQ(scTest2.normMax, 1.0);
            EXPECT_EQ(scTest2.min, 0.0);
            EXPECT_EQ(scTest2.max, 100.0);
            // compiler error std::string is not an arithmetic number
            // auto scTest3 = Scaler<std::string>(v.getObject());
            std::cout << "testing normalize..." << std::endl;
            auto norm = normalize<double>(50,0.0,1.0,0,100);
            EXPECT_EQ(0.5, norm);
            std::cout << "testing stableNormalize..." << std::endl;
            auto snorm = stableNormalize<double>(1.0,0,100);
            EXPECT_EQ(0.01, snorm);
            std::cout << "testing distanceManhattan..." << std::endl;
            auto dm = distanceManhattan<double>(2,2,6,6);
            EXPECT_EQ(80,dm);
            std::cout << "testing distanceChebyshev..." << std::endl;
            auto dch = distanceChebyshev<double>(2,2,6,6);
            EXPECT_EQ(-4,dch);
            std::cout << "testing distanceEuclidean..." << std::endl;
            float de = distanceEuclidean<double>(2,2,6,6);
            EXPECT_EQ(6,std::round(de));
            std::cout << "testing distanceCenter..." << std::endl;
            auto dc = distanceCenter<double>(5,5,10,10);
            EXPECT_EQ(2,dc);
        }
        TEST(UtilsTest, Hashing){
            std::size_t hash1 = 0u;
            hashCombine(hash1, 5);
            hashCombine(hash1, 2);
            std::size_t hash2 = 0u;
            hashCombine(hash2, 5);
            hashCombine(hash2, 2);
            auto hash = joinHashes<int>(0u, 5,2);
            EXPECT_TRUE(hash1 == hash2 && hash == hash1);
        }
        TEST(UtilsTest, Random){
            EXPECT_TRUE(randomGen().random(0.99999));
            EXPECT_FALSE(randomGen().random(0.00001));
            auto res1 = randomGen().random(0,5);
            EXPECT_TRUE(res1 >= 0 && res1 <=5);
            auto res2 = randomGen().random(0.0,5.0);
            EXPECT_TRUE(res2 >= 0.0 && res2 <=5.0);
        }
        TEST(UtilsTest, Utils){
            std::fstream file("testsData/csvTest.csv", std::ios_base::out);
            std::vector<std::vector<std::string>> csvData = {{"name","surname","dob"},
                                                             {"Pyrrha","Alexandra","1587-11-24"},
                                                             {"Patroklos","Alexandra","1588-09-13"},
                                                             {"Sophitia","Alexandra","1565-12-03"}};
            writeCSVFile(csvData, file, ',');
            file.close();
            auto readCSVAndTest = [&](){
                std::fstream csv("testsData/csvTest.csv");
                auto csvData2 = readCSVFile(csv);
                for(auto i=0u;i<csvData2.size();++i){
                    for(auto j=0u;j<csvData2[i].size();++j){
                        EXPECT_EQ(csvData2[i][j], csvData[i][j]);
                    }
                }
                csv.close();
            };
            std::cout << "it took: " << TimeThis(readCSVAndTest) << "ms to readCSVAndTest() to run." << std::endl;
            auto ffnn = createFeedForwardNN(2,3,{1,5,3},2, 1.0);
            //struct Dummy{};
            //makeJsonFrom({"dummy"}, Dummy()); // error compiling, Dummy is not meta::serializable_t
            //loadJsonFrom<Dummy>("dummyTest.json", {"dummy"}); // error compiling, Dummy is not meta::serializable_constructible_t
            makeJsonFrom({"ffnn","sc"}, *ffnn, Scaler<double>()).writeToFile("testsData/ffnnAndSCTest.json");
            auto [ffNN, sc] = loadJsonFrom<NeuralNetwork, Scaler<double>>("testsData/ffnnAndSCTest.json", {"ffnn", "sc"});
            EXPECT_EQ(ffNN, *ffnn);
            EXPECT_EQ(Scaler<double>().toJson(), sc.toJson());
#ifndef __EMSCRIPTEN__
            // test Guard if not in emscripten as it doesnt work.
            std::cout << "testing EvoAI::Guard" << std::endl;
            std::vector<int> vec;
            vec.reserve(5000);
            EvoAI::Guard<std::vector<int>*> gVec(&vec);
            std::thread t([&](){
                for(auto i=0;i<3000;++i){                
                    gVec.visit([](auto& v){
                        auto rg = EvoAI::randomGen();
                        auto r = rg.random(0, 10);
                        v->emplace_back(r);
                    });
                }
            });
            for(auto i=0;i<2000;++i){
                gVec.visit([](auto& v){
                    auto rg = EvoAI::randomGen();
                    auto r = 20 + rg.random(0, 10);
                    v->emplace_back(r);
                });
            }
            if(t.joinable()){
                t.join();
            }
            EXPECT_EQ(5000u, vec.size());
#endif
        }
        TEST(UtilsTest, NNUtils){
            std::cout << "Testing createFeedForwardNN" << std::endl;
            auto ffnn = createFeedForwardNN(2,3,{1,5,3},2, 1.0);
            ffnn = createFeedForwardNN(2,2,{1,5},2, 1.0);
            ffnn = createFeedForwardNN(2,3,{4,2,3},2, 1.0);
            ffnn = createFeedForwardNN(2,0,{0},2, 1.0);
            std::cout << "Testing createElmanNeuralNetwork" << std::endl;
            auto elman = createElmanNeuralNetwork(2,3,{1,5,3},2, 1.0);
            elman = createElmanNeuralNetwork(2,1,{1},2, 1.0);
            elman = createElmanNeuralNetwork(2,5,{1,2,1,2,3},2, 1.0);
            EXPECT_THROW(createElmanNeuralNetwork(2,0,{0},2, 1.0), std::exception);
            std::cout << "Testing createCPPN" << std::endl;
            auto cppn = createCPPN(2,3,{4,2,1},2,1.0);
            cppn = createCPPN(2,1,{3},2,1.0);
            cppn = createCPPN(2,5,{1,5,1,2,3},2,1.0);
            cppn = createCPPN(2,3,{3,1,1},2,1.0);
            cppn = createCPPN(2,11,{7,25,20,15,10,5,10,15,20,25,2},2,1.0);
            cppn = createCPPN(2,0,{},2,1.0);
            std::cout << "Testing Argmax" << std::endl;
            std::vector<double> v = {0.0, 2.0, 1.0};
            EXPECT_EQ(1u,Argmax(v));
            v = {1.0,2.0,-2.0, 2.0};
            EXPECT_EQ(1u,Argmax(v));
            EXPECT_EQ(3u, Argmax(std::begin(v) + 2, std::end(v)) + 2);
            v = {2.0,2.0,2.0};
            EXPECT_EQ(0u,Argmax(v));
        }
    }
}
#endif // EVOAI_UTILS_TEST_HPP
