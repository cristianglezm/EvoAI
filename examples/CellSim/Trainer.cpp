#include "Trainer.hpp"

namespace EvoAI{
    Trainer::Trainer(std::size_t samples, std::size_t batchSize, std::size_t epoch, double lr) noexcept
    : trainingSet(init(samples, batchSize))
    , testSet(init(10, batchSize))
    , mBatchSize(batchSize)
    , mSamples(samples)
    , mEpoch(epoch)
    , mLr(lr){}
    Genome Trainer::train(Genome&& g) noexcept{
        auto testFn = [](NeuralNetwork& nn, DataLoader<Dataset>& dl){
            Loss::Loss lossFn(Loss::MeanAbsoluteError{});
            auto correct = 0u;
            auto samples = dl.size();
            auto batchSize = dl.getBatchSize();
            auto totalAvgLoss = 0.0;
            for(auto i=0u;i<samples;++i){
                for(auto j=0u;j<batchSize;++j){
                    auto [inputs, target] = dl();
                    auto outputs = nn.forward(inputs);
                    nn.reset();
                    if(Argmax(outputs) == Argmax(target)){
                        ++correct;
                    }
                    totalAvgLoss += lossFn(target, outputs);
                }
            }
            double accuracy = 100 * (correct / static_cast<double>(samples * batchSize));
            double testAvgLoss = totalAvgLoss / (samples * batchSize);
            std::cout << "Accuracy: " << accuracy << "% testAvgLoss: " << testAvgLoss << std::endl;
            return std::make_pair(testAvgLoss, accuracy);
        };
        auto nn = Genome::makePhenotype(g);
        Optimizer<SGD, ConstantLR> optim(mLr, mBatchSize, SGD(nn.getParameters(), 0.1), Scheduler(ConstantLR()));
        nn.train(trainingSet, testSet, optim, mEpoch, Loss::MeanSquaredError{}, testFn);
        return Genome::makeGenome(nn);
    }
    void Trainer::addToTrainingSet(std::pair<std::vector<double>, std::vector<double>>&& sample) noexcept{
        trainingSet.getDataset().add(std::forward<std::pair<std::vector<double>, std::vector<double>>>(sample));
    }
    void Trainer::addToTestSet(std::pair<std::vector<double>, std::vector<double>>&& sample) noexcept{
        testSet.getDataset().add(std::forward<std::pair<std::vector<double>, std::vector<double>>>(sample));
    }
    std::pair<std::vector<double>, std::vector<double>> Trainer::generateSample() noexcept{
        // inputs
        // 0-7 sensors, 8 health, 9 stamina, 10 hunger, 11 targetAttacking, 12 targetAlive
        std::vector<double> inputs(13, 0.0);
        // targets
        // 0 idle, 1-8 move*(north, northEast, ...), 9 attacking, 10 eating, 11 defending
        std::vector<double> targets(12, 0.0);
        if(randomGen().random(0.5)){
            auto index = randomGen().random(0u, 7u);
            inputs[index] += 1.0;
            inputs[index+1] += 0.2;
            if((index+2) <= 7){
                inputs[index+2] += 0.2;
            }
        }
        for(auto i=8u;i<11;++i){
            inputs[i] = randomGen().random(0, 100) / 100.0;
        }
        inputs[11] = randomGen().random(0.5) ? 1.0:0.0;
        inputs[12] = randomGen().random(0.5) ? 1.0:0.0;
        if(inputs[12] == 0.0){
            inputs[11] = 0.0;
        }
        // set targets data
        if(inputs[9] > 0.5){
            for(auto i=0u;i<8;++i){
                if(inputs[i] > 0.5){
                    targets[i+1] += 1.0;
                    if((i+2) < 9){
                        targets[i+2] += 0.2;
                    }
                }
                targets[i] = std::clamp(targets[i], 0.0, 1.0);
            }
        }else{
            targets[0] = 1.0;
        }
        // health
        if(inputs[8] < 0.8){
            targets[10] += 0.2;
            targets[0] += 0.2;
        }else if(inputs[8] < 0.6){
            targets[0] += 0.4;
            targets[10] += 0.4;
        }else if(inputs[8] < 0.4){
            targets[0] += 0.6;
            targets[10] += 0.6;
        }else if(inputs[8] < 0.2){
            targets[0] += 0.8;
            targets[10] += 0.8;
        }
        // stamina
        if(inputs[9] < 0.8){
            targets[0] += 0.2;
        }else if(inputs[9] < 0.6){
            targets[0] += 0.4;
        }else if(inputs[9] < 0.4){
            targets[0] += 0.6;
        }else if(inputs[9] < 0.2){
            targets[0] += 0.8;
        }
        // hunger
        if(inputs[10] < 0.8){
            targets[10] += 0.2;
        }else if(inputs[10] < 0.6){
            targets[10] += 0.4;
        }else if(inputs[10] < 0.4){
            targets[10] += 0.6;
        }else if(inputs[10] < 0.2){
            targets[10] += 0.8;
        }
        // targetAttacking
        if(inputs[11] > 0.5){
            targets[11] = 1.0;
        }
        // targetAlive
        if(inputs[12] > 0.5){
            targets[10] = 1.0;
        }else{
            targets[11] = 0.0;
            targets[9] = 1.0;
        }
        targets[0] = std::clamp(targets[0], 0.0, 1.0);
        targets[10] = std::clamp(targets[10], 0.0, 1.0);
        return std::make_pair(inputs, targets);
    }
    DataLoader<Dataset> Trainer::init(std::size_t samples, std::size_t batchSize) noexcept{
        std::vector<std::pair<std::vector<double>, std::vector<double>>> data;
        for(auto i=0u;i<samples;++i){
            data.emplace_back(generateSample());
        }
        return DataLoader<Dataset>(Dataset(std::move(data), batchSize), true);
    }
}