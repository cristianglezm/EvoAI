#include <iostream>
#include <EvoAI.hpp>
#include <utility>
#include <memory>
#include <vector>

void usage() noexcept;
void testXOR(EvoAI::NeuralNetwork& nn);
void trainXOR(EvoAI::NeuralNetwork& nn);

int main(int argc, char* argv[]){
    using namespace EvoAI;
    if(argc < 2){
        usage();
        return EXIT_FAILURE;
    }
    bool trainingMode = false;
    bool evolutionMode = false;
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
        std::cout << "Evolving a population..." << std::endl;
        Population p(150,2,1,1,false,false);
        std::vector<double> x = {0.0,0.0,1.0,1.0};
        std::vector<double> y = {0.0,1.0,0.0,1.0};
        std::vector<double> truth = {0.0,1.0,1.0,0.0};
        auto errorSum = 999.0;
        auto minError = 0.1;
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
                errorSum = (std::fabs(results[0]) + std::fabs(1.0-results[1]) + std::fabs(1.0-results[2]) + std::fabs(results[3]));
                ge->setFitness(std::pow((2.0 - errorSum), 2));
            }
            if(errorSum >= minError){
                p.reproduce(false,Population::SelectionType::TOURNAMENT);
            }
        }
        std::cout << "Selecting Winner..." << std::endl;
        auto ge = p.getBestGenome();
        if(ge){
            if(saveGen){
                ge->writeToFile(savingFileGenome);
            }
            nn = Genome::makePhenotype(*ge);
            testXOR(*nn);
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
    do{
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
        nn.train(std::move(inputs),std::move(outputs),0.7,0.3,50);
    }while(nn.getMSE() > 0.01);
}
void testXOR(EvoAI::NeuralNetwork& nn){
    std::vector<double> x = {0.0,0.0,1.0,1.0};
    std::vector<double> y = {0.0,1.0,0.0,1.0};
    std::vector<double> truth = {0.0,1.0,1.0,0.0};
    for(auto i=0u;i<4;++i){
        std::vector<double> input;
        input.emplace_back(y[i]);
        input.emplace_back(x[i]);
        nn.setInputs(std::move(input));
        auto res = nn.run();
        nn.reset();
        std::cout << "x: " << x[i] << ", y: " << y[i] << " : raw answer: " << res[0] << " binary: " << (res[0] >= 0.5 ? 1:0) << " Correct Answer: " << truth[i] << std::endl;
    }
}
void usage() noexcept{
    std::cout << "Usage: XOR.exe <mode>" << std::endl;
    std::cout << "\t\t-e, --evolution\t\t\t\tTries to solve XOR evolving a population to solve the XOR." << std::endl;
    std::cout << "\t\t-t, --training\t\t\t\tTrains a neural network to solve the XOR." << std::endl;
    std::cout << "\t\t-c, --check <g|n> <filename> \t\tcheck a genome or a neural network." << std::endl;
    std::cout << "\t\t-s, --save-nn <filename>\t\tSaves the neural network." << std::endl;
    std::cout << "\t\t-sg, --save-g <filename>\t\tSaves the genome." << std::endl;
    std::cout << "\t\t-h, --help\t\t\t\tThis menu." << std::endl;
}