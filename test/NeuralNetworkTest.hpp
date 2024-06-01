#ifndef EVOAI_NEURAL_NETWORK_TEST_HPP
#define EVOAI_NEURAL_NETWORK_TEST_HPP

#include <gtest/gtest.h>
#include <EvoAI.hpp>

namespace EvoAI{
    namespace Test{
        class NeuralNetworkTest : public ::testing::Test {
            protected:
                // If the constructor and destructor are not enough for setting up
                // and cleaning up each test, you can define the following methods:
                // Code here will be called immediately after the constructor (right
                // before each test).
                NeuralNetworkTest();
                virtual void SetUp(){
                    
                }
                // Code here will be called immediately after each test (right
                // before the destructor).
                virtual void TearDown(){
                    
                }
                // You can do clean-up work that doesn't throw exceptions here.
                virtual ~NeuralNetworkTest() = default;
            public:
            NeuralNetwork nn1;
            NeuralNetwork nn2;
        };
        NeuralNetworkTest::NeuralNetworkTest() : nn1(1,1,{5},1,1.0), nn2(){}
        TEST_F(NeuralNetworkTest,DefaultConstructor){
            //layers size
            EXPECT_EQ(0u, nn2.size());
            nn2.addLayer(NeuronLayer(2,Neuron::Type::HIDDEN, 1.0));
            Neuron n;
            nn2.addNeuron(n,0);
            //layers size
            EXPECT_EQ(1u,nn2.size());
        }
        TEST_F(NeuralNetworkTest,SecondConstructor){
            //layer size
            EXPECT_EQ(3u,nn1.size());
            //layer 1 neurons size
            EXPECT_EQ(5u,nn1[1].size());
            //testing getIndex
            auto link = nn1.getIndex(&nn1[1][0]);
            EXPECT_EQ(1u,link.layer);
            EXPECT_EQ(0u,link.neuron);
            // testing removing neuron
            EXPECT_TRUE(nn1.removeNeuron(&nn1[1][0]));
            EXPECT_EQ(4u,nn1[1].size());
        }
        TEST_F(NeuralNetworkTest,RemoveNeuronWithConnections){
            auto nn = createFeedForwardNN(2,1,{3},2,1.0);
            auto& nnSelf = *nn;
            nn->writeToFile("testsData/notRemovedNeurons.json");
            EXPECT_TRUE(nn->removeNeuron(&nnSelf[1][1]));
            nn->writeToFile("testsData/removedNeurons.json");
        }
        TEST_F(NeuralNetworkTest,getNeurons){
            auto nn = createFeedForwardNN(2,1,{3},2,1.0);
            for(auto n:nn->getNeurons()){
                EXPECT_TRUE(n!=nullptr);
            }
            EXPECT_EQ(7u,nn->getNeurons().size());
            nn->addNeuron(Neuron(),1);
            EXPECT_EQ(8u,nn->getNeurons().size());
            EXPECT_TRUE(nn->removeNeuron(&(*nn)[1][1]));
            EXPECT_EQ(7u,nn->getNeurons().size());
        }
        TEST_F(NeuralNetworkTest,JsonConstructor){
            NeuralNetwork nnSave(1,2,{1,1},1,1.0);
            nnSave.writeToFile("testsData/testNN.json");
            NeuralNetwork nnJson("testsData/testNN.json");
            EXPECT_TRUE(nnJson == nnSave);
        }
        TEST_F(NeuralNetworkTest,CheckOutputs){
            NeuralNetwork nn(1,1,{1},1,1.0);
            nn[0][0].setBiasWeight(1.0);
            nn[1][0].setBiasWeight(1.0);
            nn[2][0].setBiasWeight(1.0);
            auto conn = Connection(Link(0,0),Link(1,0),1.0);
            nn.addConnection(conn);
            nn.addConnection(Connection(Link(1,0),Link(2,0),1.0));
            auto [src, dest] = nn[conn];
            EXPECT_EQ(src, nn[0][0]);
            EXPECT_EQ(dest, nn[1][0]);
            auto input = 50.0;
            // out = S(S(i1 * w1 + b1) * w2 + b2) //
            auto expectedOut = (Activations::sigmoid(Activations::sigmoid(input * 1 + 1) * 1 + 1));
            nn.setInputs({input});
            auto out = nn.run();
            nn.reset();
            std::cout << "Output: " << out[0] << " ExpectedOutput: " << expectedOut << std::endl;
            EXPECT_EQ(expectedOut,out[0]);
        }
        TEST_F(NeuralNetworkTest, CheckTrain){
            NeuralNetwork nn(1,1,{1},1,1.0);
            nn[0][0].setBiasWeight(1.0);
            nn[1][0].setBiasWeight(1.0);
            nn[2][0].setBiasWeight(1.0);
            nn.addConnection(Connection(Link(0,0),Link(1,0),1.0));
            nn.addConnection(Connection(Link(1,0),Link(2,0),1.0));
            auto batchSize = 10u;
            DataLoader<Dataset> trainDataset(Dataset({{1},{2},{3},{4},{5},{6},{7},{8},{9},{10}}, 
                                                     {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}}, batchSize));
            DataLoader<Dataset> testDataset(Dataset({{1},{2},{3},{4},{5},{6},{7},{8},{9},{10}}, 
                                                    {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}}, batchSize));
            auto testFn = [](NeuralNetwork& NN, auto& dataset){
                auto totalLoss = 0.0;
                auto samples = dataset.size();
                auto BatchSize = dataset.getBatchSize();
                auto correct = 0u;
                for(auto i=0u;i<samples;++i){
                    for(auto b=0u;b<BatchSize;++b){
                        auto [inputs, target] = dataset();
                        auto outputs = NN.forward(inputs);
                        NN.reset();
                        totalLoss += Loss::MeanSquaredError{}(target, outputs);
                        if(auto out = outputs[0] < 0.5 ? 0.0:1.0; out == target[0]){
                            ++correct;
                        }
                    }
                }
                auto accuracy = 100 * (correct / (samples * BatchSize));
                auto testAvgLoss = totalLoss / (samples * BatchSize);
                std::cout << "accuracy: " << accuracy << "% testAvgLoss: " << testAvgLoss << std::endl;
                return std::make_pair(testAvgLoss, accuracy);
            };
            EvoAI::Optimizer optim(0.1, batchSize, SGD(nn.getParameters(), 0.8), EvoAI::Scheduler(ConstantLR()));
            writeMultiPlot("testsData/checkTrainAvgLoss.txt",{"EpochAvgLoss", "testAvgLoss", "accuracy"}, 
                            nn.train(trainDataset, testDataset, optim, 50, Loss::MeanSquaredError{}, testFn));
            std::cout << "after training: " << std::endl;
            testFn(nn, testDataset);
            nn.writeDotFile("testsData/checkTrain.dot");
            nn.reset();
            for(auto i=1;i<=10;++i){
                auto out = nn.forward({static_cast<double>(i)});
                nn.reset();
                std::cout << "Output: " << out[0] << " Expected: 0" << std::endl;
                EXPECT_EQ(0,out[0] > 0.5 ? 1:0);
            }
        }
        TEST_F(NeuralNetworkTest, checkGradients){
            auto nn = EvoAI::createFeedForwardNN(2,0,{0},1, 6.8813735870195432);
            auto& nnRef = *nn;
            nnRef[0][0][0].setWeight(-3.0);
            nnRef[0][0].setBiasWeight(6.8813735870195432);
            nnRef[0][1][0].setWeight(1.0);
            nnRef[0][1].setBiasWeight(6.8813735870195432);
            nnRef[1][0].setBiasWeight(6.8813735870195432);
            nnRef[0].setActivationType(EvoAI::Neuron::ActivationType::IDENTITY);
            nnRef[1].setActivationType(EvoAI::Neuron::ActivationType::TANH);
            auto out = nn->forward({2.0,0.0});
            nn->backward({1.0});
            constexpr double epsilon = 1e-2;
            EXPECT_NEAR(nnRef[0][0].getGradient(), -1.5, epsilon); // nrn00 gradient
            EXPECT_NEAR(nnRef[0][1].getGradient(), 0.5, epsilon); // nrn01 gradient
            EXPECT_NEAR(nnRef[0][0][0].getGradient(), 1.0, epsilon); // w1 gradient
            EXPECT_NEAR(nnRef[0][0][1].getGradient(), 0.0, 1.0); // w2 gradient
            EXPECT_NEAR(nnRef[0][1][0].getGradient(), 0.5, 1.0); // nrn10 gradient
            EXPECT_NEAR(nnRef[0][1].getBiasGradient(), 0.5, 1.0); // nrn10 bias gradient
            nn->writeDotFile("testsData/NNCheckGradients.dot");
            nn->writeToFile("testsData/NNCheckGradients.json");
        }
    }
}

#endif // EVOAI_NEURAL_NETWORK_TEST_HPP
