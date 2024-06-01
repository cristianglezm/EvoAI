#include <EvoAI.hpp>

#include <iostream>
#include <utility>
#include <memory>
#include <vector>

void usage() noexcept;
/**
 * @brief evolves a population to solve XOR using NEAT
 * @param saveGen to save genome or not
 * @param savingFileGenome filename to save genome
 */
void evolveNEAT(bool saveGen, const std::string& savingFileGenome, bool binaryCross = false) noexcept;
/**
 * @brief evolves a population to solve XOR using HyperNEAT
 * @param saveGen to save genome or not
 * @param savingFileGenome filename to save genome
 */
void evolveHyperNeat(bool saveGen, const std::string& savingFileGenome, bool binaryCross = false) noexcept;
/**
 * @brief testXOR with cout comparing
 * @param nn EvoAI::NeuralNetwork&
 */
void testXOR(EvoAI::NeuralNetwork& nn);
/**
 * @brief required TestFn for EvoAI::NeuralNetwork::train
 * @param nn EvoAI::NeuralNetwork&
 * @param ds EvoAI::DataLoader<EvoAI::Dataset>&
 * @return std::pair<double, double> avgTestLoss and accuracy
 */
std::pair<double, double> testXORDataset(EvoAI::NeuralNetwork& nn, EvoAI::DataLoader<EvoAI::Dataset>& ds) noexcept;
/**
 * @brief trains XOR
 * @param nn EvoAI::NeuralNetwork&
 * @param trainDataset EvoAI::DataLoader<EvoAI::Dataset>&
 * @param testDataset EvoAI::DataLoader<EvoAI::Dataset>&
 * @param batchSize batch size
 * @param binaryCross to use BinaryCrossEntropy or MSE Loss
 */
void trainXOR(EvoAI::NeuralNetwork& nn, 
    EvoAI::DataLoader<EvoAI::Dataset>& trainDataset, 
    EvoAI::DataLoader<EvoAI::Dataset>& testDataset, std::size_t epoch, std::size_t batchSize = 4, bool binaryCross = false);

int main(int argc, char* argv[]){
    EvoAI::randomGen().setSeed(42);
    if(argc < 2){
        usage();
        return EXIT_FAILURE;
    }
    bool trainingMode = false;
    bool evolutionMode = false;
    bool hyperneat = false;
    bool checkGenome = false;
    bool checknn = false;
    bool saveGen = false;
    bool saveNN = false;
    bool binaryCross = false;
    std::string loadingFile = "file.json";
    std::string savingFileGenome = "genomeXOR.json";
    std::string savingFileNN = "nnXOR.json";
    for(auto i=0;i<argc;++i){
        std::string val(argv[i]);
        if(val == "-e" || val == "--evolve"){
            evolutionMode = true;
            auto hpn = std::string("");
            if(argv[i+1] != nullptr){
                hpn = std::string(argv[i+1]);
            }
            if(hpn == "hn"){
                hyperneat = true;
            }
        }else if(val == "-t" || val == "--train"){
            trainingMode = true;
        }else if(val == "-bc" || val =="--binaryCross"){
            binaryCross = true;
        }else if(val == "-c" || val == "--check"){
            if(std::string(argv[i+1]) == "g"){
                checkGenome = true;
                loadingFile = std::string(argv[i+2]);
            }else if(std::string(argv[i+1]) == "n"){
                checknn = true;
                loadingFile = std::string(argv[i+2]);
            }
        }else if(val == "-s" || val == "-save-nn"){
            saveNN = true;
            savingFileNN = std::string(argv[i+1]);
        }else if(val == "-sg" || val == "--save-g"){
            saveGen = true;
            savingFileGenome = std::string(argv[i+1]);
        }else if(val == "-h" || val == "--help"){
            usage();
            return EXIT_SUCCESS;
        }
    }
    std::unique_ptr<EvoAI::NeuralNetwork> nn = nullptr;
    std::unique_ptr<EvoAI::Genome> g = nullptr;
    std::vector<std::vector<double>> inputs;
    std::vector<std::vector<double>> outputs;
    {
        std::vector<double> x = {0.0,0.0,1.0,1.0};
        std::vector<double> y = {0.0,1.0,0.0,1.0};
        std::vector<double> truth = {0.0,1.0,1.0,0.0};
        for(auto i=0u;i<4;++i){
            std::vector<double> in;
            std::vector<double> out;
            in.emplace_back(x[i]);
            in.emplace_back(y[i]);
            out.emplace_back(truth[i]);
            inputs.emplace_back(in);
            outputs.emplace_back(out);
        }
    }
    auto tInputs = inputs;
    auto tOutputs = outputs;
    auto batchSize = 1;
    auto epoch = 2500;
    if(binaryCross){
        batchSize = 3;
        epoch = 5000;
    }
    EvoAI::DataLoader trainDataset(EvoAI::Dataset(std::move(inputs), std::move(outputs), batchSize));
    EvoAI::DataLoader testDataset(EvoAI::Dataset(std::move(tInputs), std::move(tOutputs), batchSize));
    if(evolutionMode){
        if(hyperneat){
            evolveHyperNeat(saveGen,savingFileGenome, binaryCross);
        }else{
            evolveNEAT(saveGen,savingFileGenome, binaryCross);
        }
    }else if(trainingMode){
        nn = EvoAI::createFeedForwardNN(2,2,{3,3},1,1.0);
        EvoAI::UniformInit(*nn);
        std::cout << "Pre-Training" << std::endl;
        testXOR(*nn);
        std::cout << "training..." << std::endl;
        trainXOR(*nn, trainDataset, testDataset, epoch, batchSize, binaryCross);
        std::cout << "Post-Training" << std::endl;
        testXOR(*nn);
    }else if(checkGenome){
        std::cout << "Checking genome..." << loadingFile << std::endl;
        g = std::make_unique<EvoAI::Genome>(loadingFile);
        nn = std::make_unique<EvoAI::NeuralNetwork>(EvoAI::Genome::makePhenotype(*g));
        testXOR(*nn);
        if(saveGen){
            g->writeToFile(savingFileGenome);
        }
    }else if(checknn){
        std::cout << "Checking Neural Network..." << loadingFile << std::endl;
        nn = std::make_unique<EvoAI::NeuralNetwork>(loadingFile);
        testXOR(*nn);
    }
    if(saveNN){
        nn->writeToFile(savingFileNN);
    }
    return EXIT_SUCCESS;
}
void trainXOR(EvoAI::NeuralNetwork& nn, 
    EvoAI::DataLoader<EvoAI::Dataset>& trainDataset, 
    EvoAI::DataLoader<EvoAI::Dataset>& testDataset, std::size_t epoch, std::size_t batchSize, bool binaryCross){
    if(binaryCross){
        // Binary Cross Entropy
        EvoAI::Optimizer optim(0.9, batchSize, EvoAI::SGD(nn.getParameters(), 0.0), EvoAI::Scheduler(EvoAI::ConstantLR()));
        EvoAI::writeMultiPlot("xorAvgLoss.txt", {"epochAvgLoss", "testAvgLoss", "accuracy"},
            nn.train(trainDataset, testDataset, optim, epoch, EvoAI::Loss::BinaryCrossEntropy{}, &testXORDataset));
    }else{
        // Mean Squared Error
        EvoAI::Optimizer optim(0.5, batchSize, EvoAI::SGD(nn.getParameters(), 0.9), EvoAI::Scheduler(EvoAI::ConstantLR()));
        EvoAI::writeMultiPlot("xorAvgLoss.txt", {"epochAvgLoss", "testAvgLoss", "accuracy"},
            nn.train(trainDataset, testDataset, optim, epoch, EvoAI::Loss::MeanSquaredError{}, &testXORDataset));
    }
    nn.writeDotFile("xor.dot");
}
std::pair<double, double> testXORDataset(EvoAI::NeuralNetwork& nn, EvoAI::DataLoader<EvoAI::Dataset>& ds) noexcept{
    auto totalLoss = 0.0;
    auto samples = ds.size();
    auto batchSize = ds.getBatchSize();
    auto correct = 0u;
    for(auto i=0u;i<samples;++i){
        for(auto b=0u;b<batchSize;++b){
            auto [inputs, target] = ds();
            auto outputs = nn.forward(inputs);
            nn.reset();
            totalLoss += EvoAI::Loss::MeanSquaredError{}(target, outputs);
            auto out = outputs[0] > 0.5 ? 1.0:0.0;
            if(out == target[0]){
                ++correct;
            }
        }
    }
    double accuracy = 100 * (correct / static_cast<double>(samples * batchSize));
    double testAvgLoss = totalLoss / static_cast<double>(samples * batchSize);
    std::cout << "accuracy: " << accuracy << "% testAvgLoss: " << testAvgLoss << std::endl;
    return std::make_pair(testAvgLoss, accuracy);
}
void testXOR(EvoAI::NeuralNetwork& nn){
    std::vector<double> x = {0.0,0.0,1.0,1.0};
    std::vector<double> y = {0.0,1.0,0.0,1.0};
    std::vector<double> truth = {0.0,1.0,1.0,0.0};
    for(auto i=0u;i<4;++i){
        std::vector<double> input;
        input.emplace_back(x[i]);
        input.emplace_back(y[i]);
        auto res = nn.forward(std::move(input));
        nn.reset();
        std::cout << "x: " << x[i] << ", y: " << y[i] << " : raw answer: " << res[0] << "\t\tbinary: " << (res[0] >= 0.5 ? 1:0) << " Correct Answer: " << truth[i] << std::endl;
    }
}
void evolveNEAT(bool saveGen, const std::string& savingFileGenome, bool binaryCross) noexcept{
    std::cout << "Evolving NEAT Neural Networks..." << std::endl;
    EvoAI::Population<EvoAI::Genome> p(500,2.0, 2.0, 1.0, 2,1);
    p.setCompatibilityThreshold(10.0);
    std::vector<double> x = {0.0,0.0,1.0,1.0};
    std::vector<double> y = {0.0,1.0,0.0,1.0};
    std::vector<double> truth = {0.0,1.0,1.0,0.0};
    auto loss = 999.0;
    auto minError = 0.1;
    auto gen = 0u;
    auto eval = [&](auto& ge){
        loss = 0.0;
        std::vector<double> results;
        ge.mutate();
        auto phenotype = EvoAI::Genome::makePhenotype(ge);
        for(auto i=0u;i<4u;++i){
            auto out = phenotype.forward({x[i],y[i]});
            results.emplace_back(out[0]);
            phenotype.reset();
        }
        if(binaryCross){
            loss = EvoAI::Loss::BinaryCrossEntropy{}(truth, results);
        }else{
            loss = EvoAI::Loss::MeanSquaredError{}(truth, results);
        }
        ge.setFitness(100.0 - loss);
    };
    while(loss >= minError){
        p.eval(eval);
        std::cout << "\rGeneration: " << gen << " - AVG Fitness: " << p.computeAvgFitness() << " NumSpecies: " << p.getSpeciesSize() << " loss: " << loss << "\t\t\t";
        std::flush(std::cout);
        if(loss >= minError){
            p.reproduce(EvoAI::SelectionAlgorithms::Tournament<EvoAI::Genome>{p.getPopulationMaxSize(), 5}, true);
            p.increaseAgeAndRemoveOldSpecies();
            p.regrowPopulation(2.0, 2.0, 1.0, 2, 1);
            ++gen;
        }else{
            std::cout << std::endl;
        }
    }
    std::cout << "Selecting Winner..." << std::endl;
    auto ge = p.getBestMember();
    if(ge){
        if(saveGen){
            ge->writeToFile(savingFileGenome);
        }
        auto nn = EvoAI::Genome::makePhenotype(*ge);
        nn.writeDotFile("xorNEAT.dot");
        testXOR(nn);
    }
}
void evolveHyperNeat(bool saveGen, const std::string& savingFileGenome, bool binaryCross) noexcept{
    std::cout << "Evolving HyperNEAT Neural Networks..." << std::endl;
     /// @warning important for the population to be cppn for HyperNEAT
    EvoAI::Population<EvoAI::Genome> p(500,2.0, 2.0, 1.0, 3, 2,false, true);
    p.setCompatibilityThreshold(10.0);
    std::vector<double> x = {0.0,0.0,1.0,1.0};
    std::vector<double> y = {0.0,1.0,0.0,1.0};
    std::vector<double> truth = {0.0,1.0,1.0,0.0};
    auto loss = 999.0;
    auto minError = 0.1;
    auto gen = 0u;
    auto eval = [&](auto& ge){
        std::vector<double> results;
        loss = 0.0;
        ge.mutate();
        auto phenotype = EvoAI::HyperNeat(EvoAI::SubstrateInfo(2,1,{3},1), ge, EvoAI::HyperNeat::SubstrateConfiguration::GRID);
        for(auto i=0u;i<4;++i){
            phenotype.setInputs({x[i],y[i]});
            auto out = phenotype.run();
            results.emplace_back(out[0]);
            phenotype.reset();
        }
        if(binaryCross){
            loss = EvoAI::Loss::BinaryCrossEntropy{}(truth, results);
        }else{
            loss = EvoAI::Loss::MeanSquaredError{}(truth, results);
        }
        ge.setFitness(100.0 - loss);
    };
    while(loss >= minError){
        p.eval(eval);
        std::cout << "\rGeneration: " << gen << " - AVG Fitness: " << p.computeAvgFitness() << " NumSpecies: " << p.getSpeciesSize() << " loss: " << loss << "\t\t\t";
        std::flush(std::cout);
        if(loss >= minError){
            p.reproduce(EvoAI::SelectionAlgorithms::Tournament<EvoAI::Genome>{p.getPopulationMaxSize(), 5}, true);
            p.increaseAgeAndRemoveOldSpecies();
            p.regrowPopulation(2.0, 2.0, 1.0, 3, 2, false, true);
            ++gen;
        }else{
            std::cout << std::endl;
        }
    }
    std::cout << "Selecting Winner..." << std::endl;
    auto ge = p.getBestMember();
    if(ge){
        if(saveGen){
            ge->writeToFile(savingFileGenome);
        }
        auto nn = EvoAI::HyperNeat(EvoAI::SubstrateInfo(2,1,{3},1), *ge, EvoAI::HyperNeat::SubstrateConfiguration::GRID);
        nn.getSubstrate().writeToFile("HyperNEAT-XOR-Substrate.json");
        nn.getSubstrate().writeDotFile("xorHyperNEAT.dot");
        testXOR(nn.getSubstrate());
    }
}
void usage() noexcept{
    std::cout << "Usage: XOR <mode>" << std::endl;
    std::cout << "\t\t-e, --evolve <hn>\t\t\tTries to solve XOR evolving a population to solve the XOR.(hn use HyperNeat instead of NEAT).\n";
    std::cout << "\t\t-t, --train\t\t\t\tTrains a neural network to solve the XOR.\n";
    std::cout << "\t\t-bc, --binaryCross\t\t\t\tUses BinaryCrossEntropy loss instead of Mean Squared Error.\n";
    std::cout << "\t\t-c, --check <g|n> <filename> \t\tcheck a genome or a neural network.\n";
    std::cout << "\t\t-s, --save-nn <filename>\t\tSaves the neural network.\n";
    std::cout << "\t\t-sg, --save-g <filename>\t\tSaves the genome.\n";
    std::cout << "\t\t-h, --help\t\t\t\tThis menu." << std::endl;
}
