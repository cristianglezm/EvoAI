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
}
int main(int argc, char **argv){
    bool optGenome = false;
    bool optNeural = false;
    if(argc < 1){
        std::cout << std::string(argv[0]) << " [options] <filename>\n";
        usage();
        return -1;
    }
    std::string val(argv[1]);
    if(val == "-g" || val == "--genome"){
        optGenome = true;
    }
    if(val == "-n" || val == "--neuralnetwork"){
        optNeural = true;
    }
    std::unique_ptr<EvoAI::NeuralNetwork> nn = nullptr;
    if(optNeural){
        std::cout << "Loading File " << argv[2] << std::endl;
        nn = std::make_unique<EvoAI::NeuralNetwork>(std::string(argv[2]));
    }else if(optGenome){
        
    }
    if(!nn){
        nn = EvoAI::createElmanNeuralNetwork(3,4,5,3,1.0);
    }
    sf::Image output;
    output.create(150,150);
    std::cout << "Saving Neural network to a json file" << std::endl;
    nn->writeToFile("ElmanNeuralNetwork.json");
    std::cout << "creating image with coordinates" << std::endl;
    EvoAI::generateImageFromCoordinates(output,nn.get(),"coords.png");
    std::cout << "creating image with color" << std::endl;
    EvoAI::generateImageFromColor(output,nn.get(),"color.png");
    return 0;
}