#include <iostream>
#include <SFML/Graphics.hpp>
#include <EvoAI/NeuralNetwork.hpp>
#include <utility>
#include <memory>
#include <cmath>
#include <EvoAI/Utils.hpp>
#include <EvoAI/Genome.hpp>
#include "imageUtils.hpp"

void usage(){
    std::cout << "-g, --genome [m|r] <filename> [<filename> with r]\tload a genome json file\n";
    std::cout << "\t\t\t\t\tWith m will mutate the genome.\n\t\t\t\t\tWith r will combine two genomes, without m or r will load the genome.\n";
    std::cout << "-G, --genome-type <type> <numHidden>\twill generate a genome of the type specified\n\t\t\t\t\t\ttypes:\n\t\t\t\t\t\t\t" <<
                                                            "0. Without hidden neurons\n\t\t\t\t\t\t\t1. With hidden neurons.\n";
    std::cout << "-n, --neuralnetwork <filename>\t\tload a neural network json file.\n";
    std::cout << "-N, --neuralnetwork-type <type> <numLayers> <numNLayers> will generate a random neural network of the type specified\n\t\t\t\t\t\ttypes:\n\t\t\t\t\t\t\t" <<
                                                            "0. CPPN\n\t\t\t\t\t\t\t1. FeedForward\n\t\t\t\t\t\t\t2. Elman Network\n";
    std::cout << "-c, --color\t\t\t\twill use color as input for the neural network (can be used with -C)\n";
    std::cout << "-C, --coords\t\t\t\twill use coordinates as input for the neural network (can be used with -c)\n";
    std::cout << "-bw\t\t\t\t\tthe output image is black and white.\n";
    std::cout << "-s, --save <filename>\t\t\twill save the neural network generated.\n";
    std::cout << "-sg, --save-genome <filename>\t\twill save the genome generated.\n";
    std::cout << "-f, --file-output <filename>\t\timage that will output.\n";
    std::cout << "-res, --resolution <width height>\twill create a image of that resolution(ignored if --image is specified).\n";
    std::cout << "--image <filename>\t\t\tload a image and generate another.\n";
    std::cout << "-r, --repeat <n>\t\t\tIt will generate the image again.(use it for recurrent nn)\n";
    std::cout << "-h, --help\t\t\t\thelp menu (This)\n";
}
int main(int argc, char **argv){
    bool optGenome = false;
    bool optGenomeType = false;
    bool optMutate = false;
    bool optReproduce = false;
    std::string genomeType = "0";
    int numHidden = 0;
    std::string genomeFile1 = "genome1.json";
    std::string genomeFile2 = "genome2.json";
    bool optNeuralType = false;
    std::string NeuralType = "0";
    bool optNeuralFile = false;
    std::string neuralFile = "nn.json";
    bool optColor = false;
    bool optCoords = false;
    bool optBW = false;
    bool optSave = false;
    std::string saveFile = "nn.json";
    bool optSaveGenome = false;
    std::string saveFileGenome = "g.json";
    std::string fileOutput = "image.png";
    int resWidth = 150;
    int resHeight = 150;
    int numLayers = 3;
    int numNLayers = 5;
    bool optImage = false;
    std::string imageInput = "image.png";
    int repeat = 1;
    if(argc < 3){
        std::cout << std::string(argv[0]) << " [options]\n";
        usage();
        return EXIT_FAILURE;
    }
    for(auto i=0;i<argc;++i){
        auto val = std::string(argv[i]);
        if(val == "-g" || val == "--genome"){
            optGenome = true;
            if(std::string(argv[i+1]) == "m"){
                optMutate = true;
                genomeFile1 = std::string(argv[i+2]);
            }else if(std::string(argv[i+1]) == "r"){
                optReproduce = true;
                genomeFile1 = std::string(argv[i+2]);
                genomeFile2 = std::string(argv[i+3]);
            }else{
                genomeFile1 = std::string(argv[i+1]);
            }
        }
        if(val == "-G" || val == "--genome-type"){
            optGenomeType = true;
            if(std::string(argv[i+1]) == "0"){
                genomeType = "0";
                numHidden = 0;
            }else if(std::string(argv[i+1]) == "1"){
                genomeType = "1";
                numHidden = std::stoi(std::string(argv[i+2]));
            }
        }
        if(val == "-n" || val == "--neuralnetwork"){
            optNeuralFile = true;
            neuralFile = std::string(argv[i+1]);
        }
        if(val == "-N" || val == "--neuralnetwork-type"){
            optNeuralType = true;
            NeuralType = std::string(argv[i+1]);
            numLayers = std::stoi(std::string(argv[i+2]));
            numNLayers = std::stoi(std::string(argv[i+3]));
        }
        if(val == "-c" || val == "--color"){
            optColor = true;
        }
        if(val == "-C" || val == "--coords"){
            optCoords = true;
        }
        if(val == "-bw"){
            optBW = true;
        }
        if(val == "-sg" || val == "--save-genome"){
            optSaveGenome = true;
            saveFileGenome = std::string(argv[i+1]);
        }
        if(val == "-s" || val == "--save"){
            optSave = true;
            saveFile = std::string(argv[i+1]);
        }
        if(val == "-res" || val == "--resolution"){
            resWidth = std::stoi(std::string(argv[i+1]));
            resHeight = std::stoi(std::string(argv[i+2]));
        }
        if(val == "-f" || val == "--file-output"){
            fileOutput = std::string(argv[i+1]);
        }
        if(val =="--image"){
            optImage = true;
            imageInput = std::string(argv[i+1]);
        }
        if(val == "-r" || val == "--repeat"){
            repeat = std::stoi(std::string(argv[i+1]));
        }
        if(val == "--help" || val == "-h"){
            std::cout << std::string(argv[0]) << " [options]\n";
            usage();
            return EXIT_FAILURE;
        }
    }
    std::unique_ptr<EvoAI::NeuralNetwork> nn = nullptr;
    std::unique_ptr<EvoAI::Genome> g = nullptr;
    if(optNeuralFile){
        std::cout << "Loading File " << argv[2] << std::endl;
        nn = std::make_unique<EvoAI::NeuralNetwork>(std::string(argv[2]));
    }else if(optNeuralType){
        if(NeuralType == "0"){
            std::cout << "Creating a CPPN with " << std::to_string(numLayers) << " layers and " << std::to_string(numNLayers) << " Neurons for each hidden layer.." << std::endl;
            if(optColor && optCoords && !optBW){
                nn = EvoAI::createCPPN(6,numLayers,numNLayers,3,1.0);
            }else if(optColor && optCoords && optBW){
                nn = EvoAI::createCPPN(6,numLayers,numNLayers,1,1.0);
            }else if((optColor || optCoords) && !optBW){
                nn = EvoAI::createCPPN(3,numLayers,numNLayers,3,1.0);
            }else if(optBW){
                nn = EvoAI::createCPPN(3,numLayers,numNLayers,1,1.0);
            }
        }else if(NeuralType == "1"){
            std::cout << "Creating a FeedForward Neural Network with " << std::to_string(numLayers) << " layers and " << std::to_string(numNLayers) << " Neurons for each hidden layer.." << std::endl;
            if(optColor && optCoords && !optBW){
                nn = EvoAI::createFeedForwardNN(6,numLayers,numNLayers,3,1.0);
            }else if(optColor && optCoords && optBW){
                nn = EvoAI::createFeedForwardNN(6,numLayers,numNLayers,1,1.0);
            }else if((optColor || optCoords) && !optBW){
                nn = EvoAI::createFeedForwardNN(3,numLayers,numNLayers,3,1.0);
            }else if(optBW){
                nn = EvoAI::createFeedForwardNN(3,numLayers,numNLayers,1,1.0);
            }
        }else if(NeuralType == "2"){
            std::cout << "Creating a Elman Neural Network with " << std::to_string(numLayers) << " layers and " << std::to_string(numNLayers) << " Neurons for each hidden layer.." << std::endl;
            if(optColor && optCoords && !optBW){
                nn = EvoAI::createElmanNeuralNetwork(6,numLayers,numNLayers,3,1.0);
            }else if(optColor && optCoords && optBW){
                nn = EvoAI::createElmanNeuralNetwork(6,numLayers,numNLayers,1,1.0);
            }else if((optColor || optCoords) && !optBW){
                nn = EvoAI::createElmanNeuralNetwork(3,numLayers,numNLayers,3,1.0);
            }else if(optBW){
                nn = EvoAI::createElmanNeuralNetwork(3,numLayers,numNLayers,1,1.0);
            }
        }
    }else if(optGenome){
        if(optMutate){
            g = std::make_unique<EvoAI::Genome>(genomeFile1);
            g->mutate();
        }else if(optReproduce){
            auto g1 = std::make_unique<EvoAI::Genome>(genomeFile1);
            auto g2 = std::make_unique<EvoAI::Genome>(genomeFile2);
            g = EvoAI::Genome::reproduce(*g1,*g2);
        }else{
            g = std::make_unique<EvoAI::Genome>(genomeFile1);
        }
        nn = EvoAI::Genome::makePhenotype(*g);
    }else if(optGenomeType){
        if(genomeType == "0"){
            if(optColor && optCoords && !optBW){
                g = std::make_unique<EvoAI::Genome>(6,3,true,true);
            }else if(optColor && optCoords && optBW){
                g = std::make_unique<EvoAI::Genome>(6,1,true,true);
            }else if((optColor || optCoords) && !optBW){
                g = std::make_unique<EvoAI::Genome>(3,3,true,true);
            }else if(optBW){
                g = std::make_unique<EvoAI::Genome>(3,1,true,true);
            }
        }else if(genomeType == "1"){
            if(optColor && optCoords && !optBW){
                g = std::make_unique<EvoAI::Genome>(6,numHidden,3,true,true);
            }else if(optColor && optCoords && optBW){
                g = std::make_unique<EvoAI::Genome>(6,numHidden,1,true,true);
            }else if((optColor || optCoords) && !optBW){
                g = std::make_unique<EvoAI::Genome>(3,numHidden,3,true,true);
            }else if(optBW){
                g = std::make_unique<EvoAI::Genome>(3,numHidden,1,true,true);
            }
        }
        nn = EvoAI::Genome::makePhenotype(*g);
    }
    if(optSave){
        std::cout << "Saving Neural Network to " << saveFile << " ..." << std::endl;
        nn->writeToFile(saveFile);
    }
    if(optSaveGenome){
        std::cout << "Saving Genome to " << saveFileGenome << "..." << std::endl;
        g->writeToFile(saveFileGenome);
    }
    sf::Image imgInput;
    if(!optImage){
        std::cout << "Creating Image with resolution " << resWidth << "x" << resHeight << std::endl;
        imgInput.create(resWidth,resHeight);
    }
    if(optColor && !optCoords && !optBW){
        std::cout << "Generating Image from color..." << std::endl;
        if(optImage){
            for(auto i=0;i<repeat;++i){
                EvoAI::generateImageFromColor(imageInput,nn.get(),fileOutput);
            }
        }else{
            for(auto i=0;i<repeat;++i){
                EvoAI::generateImageFromColor(imgInput,nn.get(),fileOutput);
            }
        }
    }else if(optColor && !optCoords && optBW){
        std::cout << "Generating Image from color with Black and white output..." << std::endl;
        if(optImage){
            for(auto i=0;i<repeat;++i){
                EvoAI::generateBWImageFromColor(imageInput,nn.get(),fileOutput);
            }
        }else{
            for(auto i=0;i<repeat;++i){
                EvoAI::generateBWImageFromColor(imgInput,nn.get(),fileOutput);
            }
        }
    }else if(!optColor && optCoords && !optBW){
        std::cout << "Generating Image from coordinates..." << std::endl;
        if(optImage){
            for(auto i=0;i<repeat;++i){
                EvoAI::generateImageFromCoordinates(imageInput,nn.get(),fileOutput);
            }
        }else{
            for(auto i=0;i<repeat;++i){
                EvoAI::generateImageFromCoordinates(imgInput,nn.get(),fileOutput);
            }
        }
    }else if(!optColor && optCoords && optBW){
        std::cout << "Generating Image from coordinates with black and white output..." << std::endl;
        if(optImage){
            for(auto i=0;i<repeat;++i){
                EvoAI::generateBWImageFromCoords(imageInput,nn.get(),fileOutput);
            }
        }else{
            for(auto i=0;i<repeat;++i){
                EvoAI::generateBWImageFromCoords(imgInput,nn.get(),fileOutput);
            }
        }
    }else if(optColor && optCoords && !optBW){
        std::cout << "Generating Image from color and coordinates..." << std::endl;
        if(optImage){
            for(auto i=0;i<repeat;++i){
                EvoAI::generateImageFromColorAndCoordinates(imageInput,nn.get(),fileOutput);
            }
        }else{
            for(auto i=0;i<repeat;++i){
                EvoAI::generateImageFromColorAndCoordinates(imgInput,nn.get(),fileOutput);
            }
        }
    }else if(optColor && optCoords && optBW){
        std::cout << "Generating Image from color and coordinates with black and white output..." << std::endl;
        if(optImage){
            for(auto i=0;i<repeat;++i){
                EvoAI::generateBWImageFromColorAndCoordinates(imageInput,nn.get(),fileOutput);
            }
        }else{
            for(auto i=0;i<repeat;++i){
                EvoAI::generateBWImageFromColorAndCoordinates(imgInput,nn.get(),fileOutput);
            }
        }
    }else{
        if(optImage){
            for(auto i=0;i<repeat;++i){
                EvoAI::generateBWImageFromCoords(imageInput,nn.get(),fileOutput);
            }
        }else{
            for(auto i=0;i<repeat;++i){
                EvoAI::generateBWImageFromCoords(imgInput,nn.get(),fileOutput);
            }
        }
    }
    return 0;
}