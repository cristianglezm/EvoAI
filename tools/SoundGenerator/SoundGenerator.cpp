#include <iostream>
#include <SFML/Graphics.hpp>
#include <EvoAI/NeuralNetwork.hpp>
#include <utility>
#include <memory>
#include <cmath>
#include <EvoAI/Utils.hpp>

void usage(){
    std::cout << "-g, --genome <filename> load a genome json file.\n";
    std::cout << "-n, --neuralnetwork <filename> load a neural network json file.\n";
    std::cout << "--image <filename> load a image and generate another.\n";
    std::cout << "-h, --help help menu (This)\n";
}
int main(int argc, char **argv){
    bool optGenome = false;
    bool optNeural = false;
    bool optImage = false;
    std::string val = "";
    if(argc >= 3){
        val = argv[1];
    }else if(argc == 2){
        val = argv[1];
    }
    if(val == "-g" || val == "--genome"){
        optGenome = true;
    }else if(val == "-n" || val == "--neuralnetwork"){
        optNeural = true;
    }else if(val =="--image"){
        optImage = true;
    }else if(val == "--help" || val == "-h"){
        std::cout << std::string(argv[0]) << " [options] <filename>\n";
        usage();
        return -1;
    }
    std::unique_ptr<EvoAI::NeuralNetwork> nn = nullptr;
    if(optNeural){
        std::cout << "Loading File " << argv[2] << std::endl;
        nn = std::make_unique<EvoAI::NeuralNetwork>(std::string(argv[2]));
    }else if(optGenome){
        
    }else if(optImage){
        
    }
    if(!nn){
        nn = EvoAI::createFeedForwardNN(3,3,5,3,1.0);
    }
    std::cout << "creating sound with coords image..." << std::endl;
    EvoAI::generateSoundFromColor("test.jpg",nn.get(),"sound.ogg");
    EvoAI::generateSoundFromCoordinates(150,150,nn.get(),"soundCoords.ogg");
    return 0;
}