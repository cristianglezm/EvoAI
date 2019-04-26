#include <iostream>
#include <EvoAI.hpp>
#include <utility>
#include <memory>
#include <vector>

using namespace EvoAI;

void usage() noexcept;
void evolveNEAT(bool saveGen, const std::string& savingFileGenome) noexcept;
void evolveHyperNeat(bool saveGen, const std::string& savingFileGenome) noexcept;
void testXOR(EvoAI::NeuralNetwork& nn);
void trainXOR(EvoAI::NeuralNetwork& nn);

int main(int argc, char* argv[]){
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
    std::string loadingFile = "file.json";
    std::string savingFileGenome = "genomeXOR.json";
    std::string savingFileNN = "nnXOR.json";
    for(auto i=0;i<argc;++i){
        std::string val(argv[i]);
        if(val == "-e" || val == "--evolution"){
            evolutionMode = true;
            auto hpn = std::string("");
            if(argv[i+1] != nullptr){
                hpn = std::string(argv[i+1]);
            }
            if(hpn == "hn"){
                hyperneat = true;
            }
        }else if(val == "-t" || val == "--training"){
            trainingMode = true;
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
    std::unique_ptr<NeuralNetwork> nn = nullptr;
    std::unique_ptr<Genome> g = nullptr;
    if(trainingMode){
        nn = createFeedForwardNN(2,2,3,1,1.0);
        std::cout << "Pre-Training" << std::endl;
        testXOR(*nn);
        std::cout << "training..." << std::endl;
        trainXOR(*nn);
        std::cout << "Post-Training" << std::endl;
        testXOR(*nn);
    }else if(evolutionMode){
        if(hyperneat){
            evolveHyperNeat(saveGen,savingFileGenome);
        }else{
            evolveNEAT(saveGen,savingFileGenome);
        }
    }else if(checkGenome){
        std::cout << "Checking genome..." << loadingFile << std::endl;
        g = std::make_unique<Genome>(loadingFile);
        nn = Genome::makePhenotype(*g);
        testXOR(*nn);
        if(saveGen){
            g->writeToFile(savingFileGenome);
        }
    }else if(checknn){
        std::cout << "Checking Neural Network..." << loadingFile << std::endl;
        nn = std::make_unique<NeuralNetwork>(loadingFile);
        testXOR(*nn);
    }
    if(saveNN){
        nn->writeToFile(savingFileNN);
    }
    return EXIT_SUCCESS;
}
void trainXOR(EvoAI::NeuralNetwork& nn){
    std::vector<double> x = {0.0,0.0,1.0,1.0};
    std::vector<double> y = {0.0,1.0,0.0,1.0};
    std::vector<double> truth = {0.0,1.0,1.0,0.0};
    std::vector<std::vector<double>> inputs;
    std::vector<std::vector<double>> outputs;
    for(auto i=0u;i<4;++i){
        std::vector<double> in;
        std::vector<double> out;
        in.emplace_back(x[i]);
        in.emplace_back(y[i]);
        out.emplace_back(truth[i]);
        inputs.emplace_back(in);
        outputs.emplace_back(out);
    }
    nn.train(std::move(inputs),std::move(outputs),0.7,0.3,5000);
}
void testXOR(EvoAI::NeuralNetwork& nn){
    std::vector<double> x = {0.0,0.0,1.0,1.0};
    std::vector<double> y = {0.0,1.0,0.0,1.0};
    std::vector<double> truth = {0.0,1.0,1.0,0.0};
    for(auto i=0u;i<4;++i){
        std::vector<double> input;
        input.emplace_back(x[i]);
        input.emplace_back(y[i]);
        nn.setInputs(std::move(input));
        auto res = nn.run();
        nn.reset();
        std::cout << "x: " << x[i] << ", y: " << y[i] << " : raw answer: " << res[0] << "\t\tbinary: " << (res[0] >= 0.5 ? 1:0) << " Correct Answer: " << truth[i] << std::endl;
    }
    std::cout << "altering order..." << std::endl;
    for(auto i=3;i>-1;--i){
        std::vector<double> input;
        input.emplace_back(x[i]);
        input.emplace_back(y[i]);
        nn.setInputs(std::move(input));
        auto res = nn.run();
        nn.reset();
        std::cout << "x: " << x[i] << ", y: " << y[i] << " : raw answer: " << res[0] << "\t\tbinary: " << (res[0] >= 0.5 ? 1:0) << " Correct Answer: " << truth[i] << std::endl;
    }
}
void evolveNEAT(bool saveGen, const std::string& savingFileGenome) noexcept{
    std::cout << "Evolving NEAT Neural Networks..." << std::endl;
    Population p(500,2,1);
    std::vector<double> x = {0.0,0.0,1.0,1.0};
    std::vector<double> y = {0.0,1.0,0.0,1.0};
    std::vector<double> truth = {0.0,1.0,1.0,0.0};
    auto errorSum = 999.0;
    auto minError = 0.1;
    auto gen = 0u;
    while(errorSum >= minError){
        p.orderGenomesByFitness();
        for(auto& ge:p.getGenomes()){
            std::vector<double> results;
            ge->mutate();
            auto phenotype = Genome::makePhenotype(*ge);
            for(auto i=0u;i<4;++i){
                phenotype->setInputs({x[i],y[i]});
                auto out = phenotype->run();
                results.push_back(out[0]);
                phenotype->reset();
            }
            errorSum = (std::fabs(0.0 - results[0]) +
                        std::fabs(1.0 - results[1]) +
                        std::fabs(1.0 - results[2]) +
                        std::fabs(0.0 - results[3]));
            ge->setFitness(std::pow((4.0 - errorSum), 2));
        }
        std::cout << "\rGeneration: " << gen << " - AVG Fitness: " << p.computeAvgFitness() << " NumSpecies: " << p.getSpeciesSize() << " Error: " << errorSum << "            ";
        std::flush(std::cout);
        if(errorSum >= minError){
            p.reproduce(true,Population::SelectionType::TOURNAMENT);
            ++gen;
        }else{
            std::cout << std::endl;
        }
    }
    std::cout << "Selecting Winner..." << std::endl;
    auto ge = p.getBestGenome();
    if(ge){
        if(saveGen){
            ge->writeToFile(savingFileGenome);
        }
        auto nn = Genome::makePhenotype(*ge);
        testXOR(*nn);
    }
}
void evolveHyperNeat(bool saveGen, const std::string& savingFileGenome) noexcept{
    std::cout << "Evolving HyperNEAT Neural Networks..." << std::endl;
    Population p(500,3,2,false, true); // important for the population to be cppn for HyperNEAT
    std::vector<double> x = {0.0,0.0,1.0,1.0};
    std::vector<double> y = {0.0,1.0,0.0,1.0};
    std::vector<double> truth = {0.0,1.0,1.0,0.0};
    auto errorSum = 999.0;
    auto minError = 0.1;
    auto gen = 0u;
    while(errorSum >= minError){
        p.orderGenomesByFitness();
        for(auto& ge:p.getGenomes()){
            std::vector<double> results;
            ge->mutate();
            auto phenotype = HyperNeat(SubstrateInfo(2,1,3,1),*ge,HyperNeat::SubstrateConfiguration::GRID);
            for(auto i=0u;i<4;++i){
                phenotype.setInputs({x[i],y[i]});
                auto out = phenotype.run();
                results.push_back(out[0]);
                phenotype.reset();
            }
            errorSum = (std::fabs(0.0 - results[0]) +
                        std::fabs(1.0 - results[1]) +
                        std::fabs(1.0 - results[2]) +
                        std::fabs(0.0 - results[3]));
            ge->setFitness(std::pow((4.0 - errorSum), 2));
        }
        std::cout << "\rGeneration: " << gen << " - AVG Fitness: " << p.computeAvgFitness() << " NumSpecies: " << p.getSpeciesSize() << " Error: " << errorSum << "        ";
        std::flush(std::cout);
        if(errorSum >= minError){
            p.reproduce(true,Population::SelectionType::TRUNCATION);
            ++gen;
        }else{
            std::cout << std::endl;
        }
    }
    std::cout << "Selecting Winner..." << std::endl;
    auto ge = p.getBestGenome();
    if(ge){
        if(saveGen){
            ge->writeToFile(savingFileGenome);
        }
        auto nn = HyperNeat(SubstrateInfo(2,1,3,1),*ge,HyperNeat::SubstrateConfiguration::GRID);
        nn.getSubstrate().writeToFile("HyperNEAT-XOR-NN.json");
        testXOR(nn.getSubstrate());
    }
}
void usage() noexcept{
    std::cout << "Usage: XOR <mode>" << std::endl;
    std::cout << "\t\t-e, --evolution <hn>\t\t\t\tTries to solve XOR evolving a population to solve the XOR." << std::endl;
    std::cout << "\t\t-t, --training\t\t\t\tTrains a neural network to solve the XOR." << std::endl;
    std::cout << "\t\t-c, --check <g|n> <filename> \t\tcheck a genome or a neural network." << std::endl;
    std::cout << "\t\t-s, --save-nn <filename>\t\tSaves the neural network." << std::endl;
    std::cout << "\t\t-sg, --save-g <filename>\t\tSaves the genome." << std::endl;
    std::cout << "\t\t-h, --help\t\t\t\tThis menu." << std::endl;
}