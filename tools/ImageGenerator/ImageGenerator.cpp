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
    if(argc != 0){
        usage();
        //return -1;
    }
    std::string val(argv[0]);
    if(val == "-g" || val == "--genome"){
        optGenome = true;
    }
    if(val == "-n" || val == "--neuralnetwork"){
        optNeural = true;
    }
    std::unique_ptr<EvoAI::NeuralNetwork> nn = EvoAI::createFeedForwardNN(3,4,5,3,1.0);
    (*nn)[1].setActivationType(EvoAI::NeuronLayer::NOISY_RELU);
    (*nn)[2].setActivationType(EvoAI::NeuronLayer::SINUSOID);
    (*nn)[3].setActivationType(EvoAI::NeuronLayer::NOISY_RELU);
    sf::Image output;
    output.create(150,150);
    std::cout << "Saving Neural network to a json file" << std::endl;
    nn->writeToFile("FeedForward.json");
    std::cout << "creating image with coordinates" << std::endl;
    EvoAI::generateImageFromCoordinates(output,nn.get(),"coords.png");
    std::cout << "creating image with color" << std::endl;
    EvoAI::generateImageFromColor(output,nn.get(),"color.png");
    return 0;
}