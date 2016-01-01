#include <iostream>
#include <SFML/Graphics.hpp>
#include <EvoAI/NeuralNetwork.hpp>
#include <EvoAI/NNRenderer.hpp>
#include <utility>
#include <memory>
#include <EvoAI/Utils.hpp>

int main(int argc, char **argv){
    using EvoAI::NeuralNetwork;
    using EvoAI::Connection;
    using EvoAI::NNRenderer;
    std::unique_ptr<NeuralNetwork> nn = EvoAI::CreateFeedForwardNN(1,2,3,1,1.0);
    std::unique_ptr<NNRenderer> nr = std::make_unique<NNRenderer>(nn.get());
    sf::RenderWindow App(sf::VideoMode(1270, 720), "Neural Network Visualizer");
    while (App.isOpen()){
        sf::Event Event;
        while(App.pollEvent(Event)){
            if (Event.type == sf::Event::Closed){
                App.close();
            }
        }
        App.clear(sf::Color::Black);
        nr->Render(App);
        App.display();
    }
    return 0;
}
