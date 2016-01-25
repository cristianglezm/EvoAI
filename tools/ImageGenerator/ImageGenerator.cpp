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
    std::cout << "-bw\t\t\t\t\tthe output image is black and white.\n";
    std::cout << "-s, --save <filename>\t\t\twill save the neural network generated.\n";
    std::cout << "-f, --file-output <filename>\t\timage that will output.\n";
    std::cout << "-res, --resolution <width height>\twill create a image of that resolution.\n";
    std::cout << "--image <filename>\t\t\tload a image and generate another.\n";
    std::cout << "-h, --help\t\t\t\thelp menu (This)\n";
}
int main(int argc, char **argv){
    bool optGenome = false;
    std::string genomeFile = "";
    bool optNeuralType = false;
    std::string NeuralType = "0";
    bool optNeuralFile = false;
    std::string neuralFile = "";
    bool optColor = false;
    bool optCoords = false;
    bool optBW = false;
    bool optSave = false;
    std::string saveFile = "";
    bool optFileOutput = false;
    std::string fileoutput = "";
    bool optRes = false;
    std::string resWidth = "0", resHeight = "0";
    bool optImage = false;
    std::string imageInput = "";
    std::string val = "";
    if(argc >= 3){
        val = argv[1];
    }else if(argc == 2){
        val = argv[1];
    }
    if(val == "-g" || val == "--genome"){
        optGenome = true;
    }else if(val == "-n" || val == "--neuralnetwork"){
        optNeuralFile = true;
    }else if(val =="--image"){
        optImage = true;
    }else if(val == "--help" || val == "-h"){
        std::cout << std::string(argv[0]) << " [options] <filename>\n";
        usage();
        return -1;
    }
    std::unique_ptr<EvoAI::NeuralNetwork> nn = nullptr;
    if(optNeuralFile){
        std::cout << "Loading File " << argv[2] << std::endl;
        nn = std::make_unique<EvoAI::NeuralNetwork>(std::string(argv[2]));
    }else if(optGenome){
        
    }else if(optImage){
        
    }
    if(!nn){
        nn = EvoAI::createCPPN(3,3,5,3,1.0);
        std::cout << "Saving Neural network to a json file" << std::endl;
        nn->writeToFile("CPPN.json");
    }
    sf::Image output;
    output.create(150,150);
    std::cout << "creating image with color..." << std::endl;
    EvoAI::generateImageFromColor(output,nn.get(),"color.png");
    std::cout << "creating image with coordinates..." << std::endl;
    EvoAI::generateImageFromCoordinates(output,nn.get(),"coords.png");
    std::cout << "creating image with color And Coordinates..." << std::endl;
    auto nnSize = nn->size()-1;
    nn->removeNeuron(&(*nn)[nnSize][2]);
    nn->removeNeuron(&(*nn)[nnSize][1]);
    nn->writeToFile("CPPNWith1Output.json");
    std::cout << "creating Black And White image with Color..." << std::endl;
    EvoAI::generateBWImageFromColor(output,nn.get(),"BWTestImage.png");
    std::cout << "creating Black And White image with Coords..." << std::endl;
    EvoAI::generateBWImageFromCoords(output,nn.get(),"BWTestCoords.png");
    nn = EvoAI::createCPPN(6,3,5,3,1.0); //std::make_unique<EvoAI::NeuralNetwork>("ElmanNeuralNetwork.json");
    std::cout << "Creating Color And Coordinates..." << std::endl;
    nn->writeToFile("CPPNWith6Inputs.json");
    EvoAI::generateImageFromColorAndCoordinates(output,nn.get(),"colorAndCoordinates1.png");
    EvoAI::generateImageFromColorAndCoordinates(output,nn.get(),"colorAndCoordinates2.png");
    return 0;
}