#include <iostream>
#include <SFML/Graphics.hpp>
#include <EvoAI/NeuralNetwork.hpp>
#include <utility>
#include <memory>
#include <cmath>
#include <EvoAI/Utils.hpp>

void usage(){
    std::cout << "-g, --genome <filename>\t\t\tload a genome json file.\n";
    std::cout << "-n, --neuralnetwork <filename>\t\tload a neural network json file.\n";
    std::cout << "-N, --neuralnetwork-type <type>\t\twill generate a random neural network of the type specified\n\t\t\t\t\t\ttypes:\n\t\t\t\t\t\t\t" <<
                                                            "0. CPPN\n\t\t\t\t\t\t\t1. FeedForward\n\t\t\t\t\t\t\t2. Elman Network\n";
    std::cout << "-c, --color\t\t\t\twill use color as input for the neural network (can be used with -C)\n";
    std::cout << "-C, --coords\t\t\t\twill use coordinates as input for the neural network (can be used with -c)\n";
    std::cout << "-s, --save <filename>\t\t\twill save the neural network generated.\n";
    std::cout << "-f, --file-output <filename>\t\tsound that will output.\n";
    std::cout << "-res, --resolution <width height>\tIt will create a sound using the coordinates(ignored if --image is specified).\n";
    std::cout << "--image <filename>\t\t\tload a image and generate a sound fromt it.\n";
    std::cout << "-r, --repeat <n>\t\t\tIt will generate the sound again.(use it for recurrent nn)\n";
    std::cout << "-h, --help\t\t\t\thelp menu (This)\n";
}
int main(int argc, char **argv){
    bool optGenome = false;
    std::string genomeFile = "genome.json";
    bool optNeuralType = false;
    std::string NeuralType = "0";
    bool optNeuralFile = false;
    std::string neuralFile = "nn.json";
    bool optColor = false;
    bool optCoords = false;
    bool optSave = false;
    std::string saveFile = "nn.json";
    std::string fileOutput = "image.png";
    int resWidth = 150;
    int resHeight = 150;
    bool optImage = false;
    std::string imageInput = "image.png";
    int repeat = 1;
    if(argc < 3){
        std::cout << std::string(argv[0]) << " [options] <filename>\n";
        usage();
        return EXIT_FAILURE;
    }
    for(auto i=0;i<argc;++i){
        auto val = std::string(argv[i]);
        if(val == "-g" || val == "--genome"){
            optGenome = true;
            genomeFile = std::string(argv[i+1]);
        }
        if(val == "-n" || val == "--neuralnetwork"){
            optNeuralFile = true;
            neuralFile = std::string(argv[i+1]);
        }
        if(val == "-N" || val == "--neuralnetwork-type"){
            optNeuralType = true;
            NeuralType = std::string(argv[i+1]);
        }
        if(val == "-c" || val == "--color"){
            optColor = true;
        }
        if(val == "-C" || val == "--coords"){
            optCoords = true;
        }
        if(val == "-s" || val == "--save"){
            optSave = true;
            saveFile = std::string(argv[i+1]);
        }
        if(val == "-f" || val == "--file-output"){
            fileOutput = std::string(argv[i+1]);
        }
        if(val == "-res" || val == "--resolution"){
            resWidth = std::stoi(std::string(argv[i+1]));
            resHeight = std::stoi(std::string(argv[i+2]));
        }
        if(val =="--image"){
            optImage = true;
            imageInput = std::string(argv[i+1]);
        }
        if(val == "-r" || val == "--repeat"){
            repeat = std::stoi(std::string(argv[i+1]));
        }
        if(val == "--help" || val == "-h"){
            std::cout << std::string(argv[0]) << " [options] <filename>\n";
            usage();
            return EXIT_FAILURE;
        }
    }
    std::unique_ptr<EvoAI::NeuralNetwork> nn = nullptr;
    if(optNeuralFile){
        std::cout << "Loading File " << argv[2] << std::endl;
        nn = std::make_unique<EvoAI::NeuralNetwork>(std::string(argv[2]));
    }else if(optNeuralType){
        if(NeuralType == "0"){
            std::cout << "Creating a CPPN..." << std::endl;
            if(optColor && optCoords){
                nn = EvoAI::createCPPN(6,3,5,3,1.0);
            }else{
                nn = EvoAI::createCPPN(3,3,5,3,1.0);
            }
        }else if(NeuralType == "1"){
            std::cout << "Creating a FeedForward Neural Network..." << std::endl;
            if(optColor && optCoords){
                nn = EvoAI::createFeedForwardNN(6,3,5,3,1.0);
            }else{
                nn = EvoAI::createFeedForwardNN(3,3,5,3,1.0);
            }
        }else if(NeuralType == "2"){
            std::cout << "Creating a Elman Neural Network..." << std::endl;
            if(optColor && optCoords){
                nn = EvoAI::createElmanNeuralNetwork(6,3,5,3,1.0);
            }else{
                nn = EvoAI::createElmanNeuralNetwork(3,3,5,3,1.0);
            }
        }
    }else if(optGenome){
        std::cout << "Not yet Implemented." << std::endl; /// TODO
        return EXIT_FAILURE;
    }
    if(optSave){
        std::cout << "Saving Neural Network to " << saveFile << " ..." << std::endl;
        nn->writeToFile(saveFile);
    }
    sf::Image imgInput;
    if(!optImage){
        std::cout << "Creating Image with resolution " << resWidth << "x" << resHeight << std::endl;
        imgInput.create(resWidth,resHeight);
    }
    if(optColor && !optCoords){
        std::cout << "Generating Sound from color..." << std::endl;
        if(optImage){
            for(auto i=0;i<repeat;++i){
                EvoAI::generateSoundFromColor(imageInput,nn.get(),fileOutput);
            }
        }else{
            for(auto i=0;i<repeat;++i){
                EvoAI::generateSoundFromColor(imgInput,nn.get(),fileOutput);
            }
        }
    }else if(!optColor && optCoords){
        std::cout << "Generating sound from Coordinates..." << std::endl;
        for(auto i=0;i<repeat;++i){
            EvoAI::generateSoundFromCoordinates(resWidth,resHeight,nn.get(),fileOutput);
        }
    }else if(optColor && optCoords){
        std::cout << "Generating sound from color and coordinates" << std::endl;
        if(optImage){
            for(auto i=0;i<repeat;++i){
                EvoAI::generateSoundFromColorAndCoordinates(imageInput,nn.get(),fileOutput);
            }
        }else{
            for(auto i=0;i<repeat;++i){
                EvoAI::generateSoundFromColorAndCoordinates(imgInput,nn.get(),fileOutput);
            }
        }
    }
    return 0;
}