#include <iostream>
#include <SFML/Graphics.hpp>
#include <utility>
#include <memory>
#include <chrono>
#include <EvoAI/NeuralNetwork.hpp>
#include <EvoAI/Utils.hpp>
#include "NNRenderer.hpp"

int main(int argc, char **argv){
    using EvoAI::NeuralNetwork::NeuralNetwork;
    using EvoAI::NeuralNetwork::Connection;
    using EvoAI::NNRenderer;
    bool running = true;
    bool showText = false;
    if(argc < 2){
        std::cout << "usage:\n";
        std::cout << argv[0] << " <filename>" << std::endl;
        return EXIT_FAILURE;
    }
    // build network
    std::unique_ptr<NeuralNetwork> nn = std::make_unique<NeuralNetwork>(std::string(argv[1]));
    // build renderer for the nn.
    std::unique_ptr<NNRenderer> nr = std::make_unique<NNRenderer>(nn.get());
    sf::RenderWindow App(sf::VideoMode(1270, 720), "Neural Network Visualizer - " + std::string(argv[1]));
    while (running){
        sf::Event event;
        while(App.pollEvent(event)){
            switch(event.type){
                case sf::Event::Closed:
                        running = false;
                    break;
                case sf::Event::KeyReleased:
                    if(event.key.code == sf::Keyboard::Add){
                        sf::View v = App.getView();
                        v.zoom(0.5);
                        App.setView(v);
                    }
                    if(event.key.code == sf::Keyboard::Subtract){
                        sf::View v = App.getView();
                        v.zoom(1.5);
                        App.setView(v);
                    }
                    break;
                case sf::Event::KeyPressed:
                    if(event.key.code == sf::Keyboard::Down){
                        auto v = App.getView();
                        v.move(0,100);
                        App.setView(v);
                    }
                    if(event.key.code == sf::Keyboard::Up){
                        auto v = App.getView();
                        v.move(0,-100);
                        App.setView(v);
                    }
                    if(event.key.code == sf::Keyboard::Right){
                        auto v = App.getView();
                        v.move(100,0);
                        App.setView(v);
                    }
                    if(event.key.code == sf::Keyboard::Left){
                        auto v = App.getView();
                        v.move(-100,0);
                        App.setView(v);
                    }
                    if(event.key.code == sf::Keyboard::T){
                        showText = !showText;
                    }
                    break;
                default:
                    break;
            }
        }
        App.clear(sf::Color::Black);
        nr->Render(App,showText);
        App.display();
    }
    App.close();
    return 0;
}
