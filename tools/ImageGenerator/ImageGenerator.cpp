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
    sf::Image output;
    int width = 150; //output.getSize().x;
    int height = 150; //output.getSize().y;
    output.create(width,height); //output.loadFromFile(std::string(argv[1]));
    std::unique_ptr<EvoAI::NeuralNetwork> nn = EvoAI::createFeedForwardNN(3,2,30,3,1.0);
    (*nn)[1].setActivationType(EvoAI::NeuronLayer::NOISY_RELU);
    (*nn)[2].setActivationType(EvoAI::NeuronLayer::SINUSOID);
    (*nn)[3].setActivationType(EvoAI::NeuronLayer::RELU);
    std::cout << "creating image" << std::endl;
    for(auto x=0;x<width;++x){
        for(auto y=0;y<height;++y){
            auto norm_x = (2*(x/width))-1;
            auto norm_y = (2*(y/height))-1;
            auto d = std::sqrt(((norm_x/2)^2) + ((norm_y /2)^2));
            std::vector<double> inputs;
            inputs.emplace_back((double)x);
            inputs.emplace_back((double)y);
            inputs.emplace_back((double)d);
            nn->setInputs(std::move(inputs));
            auto color = nn->run();
            nn->reset();
            output.setPixel(x,y,sf::Color(color[0] * 255,color[1] * 255,color[2] * 255));
        }
    }
    output.saveToFile("test.png");
    return 0;
}