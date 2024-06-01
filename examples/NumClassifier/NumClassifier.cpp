#include <EvoAI/Loss.hpp>
#include <EvoAI.hpp>
#include <EvoAI/DataLoader.hpp>
#include <fstream>

namespace EvoAI{
    /**
     * @brief Dataset for numbers classification 1-3
     */
    struct NumberClassDataset{
        NumberClassDataset(std::size_t samples, std::size_t batchSize)
        : m_index(0)
        , m_samples(samples)
        , m_batchSize(batchSize)
        , m_dataset(make_dataset(samples)){}
        std::vector<std::pair<std::vector<double>, std::vector<double>>> make_dataset(std::size_t samples) noexcept{
            std::vector<std::pair<std::vector<double>, std::vector<double>>> data;
            data.reserve(samples);
            for(auto i=0u;i<samples;++i){
                auto n = randomGen().random(1u, 3u);
                switch(n){
                    case 1:{
                        data.emplace_back(std::make_pair<std::vector<double>, std::vector<double>>({1}, {1.0, 0.0, 0.0}));
                    }
                    break;
                    case 2:{
                        data.emplace_back(std::make_pair<std::vector<double>, std::vector<double>>({2}, {0.0, 1.0, 0.0}));
                    }
                    break;
                    case 3:{
                        data.emplace_back(std::make_pair<std::vector<double>, std::vector<double>>({3}, {0.0, 0.0, 1.0}));
                    }
                    break;
                    default:
                        data.emplace_back(std::make_pair<std::vector<double>, std::vector<double>>({static_cast<double>(n)}, {0.0, 0.0, 0.0}));
                    break;
                }
            }
            return data;
        }
        const std::pair<std::vector<double>&, std::vector<double>&> operator()() noexcept{
            auto i = m_index;
            m_index = (m_index + 1) % m_samples;
            return std::make_pair(std::ref(m_dataset[i].first), std::ref(m_dataset[i].second));
        }
        std::size_t size() const noexcept{
            return (m_dataset.size() + m_batchSize - 1) / m_batchSize;
        }
        std::size_t getBatchSize() const noexcept{
            return m_batchSize;
        }
        void shuffle() noexcept{
            auto g = EvoAI::randomGen().getEngine();
            std::uniform_int_distribution ud(0, static_cast<int>(m_dataset.size() - 1));
            for(auto i=0u;i<m_dataset.size();++i){
                auto index1 = ud(g);
                auto index2 = ud(g);
                std::swap(m_dataset[index1], m_dataset[index2]);
            }
        }
        // data
        mutable std::size_t m_index;
        std::size_t m_samples;
        std::size_t m_batchSize;
        std::vector<std::pair<std::vector<double>, std::vector<double>>> m_dataset;
    };
    std::pair<double, double> testNN(NeuralNetwork& nn, DataLoader<NumberClassDataset>& ds) noexcept{
        double error = 0.0;
        double totalLoss = 0.0;
        for(auto i=0u;i<ds.size();++i){
            for(auto j=0u;j<ds.getBatchSize();++j){
                auto [inputs, expectedOutputs] = ds();
                auto outputs = nn.forward(inputs);
                nn.reset();
                totalLoss += Loss::MultiClassCrossEntropy{}(expectedOutputs,outputs);
                auto pred = Argmax(outputs);
                if(pred != Argmax(expectedOutputs)){
                    error += 1.0;
                }
                auto isCorrect = pred == Argmax(expectedOutputs) ? "v/":"X";
                std::cout << "input -> " << inputs[0] << " :: " << (pred+1) <<" = " << (Argmax(expectedOutputs)+1) <<"\t" << isCorrect << "\n";
            }
        }
        auto avgLoss = totalLoss / (ds.size() * ds.getBatchSize());
        error /= (ds.size() * ds.getBatchSize());
        error *= 100;
        std::cout << " error: " << error  << "% " << "accuracy: " << (100.0 - error)  << "%" << std::endl;
        std::cout << " avgLoss: " << avgLoss << std::endl; 
        return std::make_pair(avgLoss, 100.0 - error);
    }
}

int main(){
    EvoAI::randomGen().setSeed(42);
    auto nn = EvoAI::createFeedForwardNN(1,2,{6, 6},3, 1.0);
    EvoAI::UniformInit(*nn);
    (*nn)[1].setActivationType(EvoAI::Neuron::ActivationType::RELU);
    (*nn)[2].setActivationType(EvoAI::Neuron::ActivationType::IDENTITY);
    (*nn)[3].setActivationType(EvoAI::Neuron::ActivationType::SOFTMAX);
    std::cout << "before Training:"<< std::endl;
    auto batchSize = 10u;
    EvoAI::DataLoader<EvoAI::NumberClassDataset> trainingData(EvoAI::NumberClassDataset(100, batchSize));
    EvoAI::DataLoader<EvoAI::NumberClassDataset> testingData(EvoAI::NumberClassDataset(10, batchSize));
    testNN(*nn, testingData);
    EvoAI::Optimizer optim(0.1, batchSize, EvoAI::SGD(nn->getParameters(), 0.0), EvoAI::Scheduler(EvoAI::MultiStepLR({25}, 0.1)));
    EvoAI::writeMultiPlot("NumbersClassLossPlot.txt", {"epochAvgLoss", "testAvgLoss", "accuracy"}, 
                    nn->train(trainingData, testingData, optim, 100, EvoAI::Loss::MultiClassCrossEntropy{}, &EvoAI::testNN));
    nn->writeDotFile("numClass.dot");
    std::cout << "after Training:"<< std::endl;
    testNN(*nn, testingData);
    nn->writeToFile("NumberClass.json");
    return 0;
}