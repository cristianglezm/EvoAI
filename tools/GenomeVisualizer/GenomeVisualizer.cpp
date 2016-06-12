#include <iostream>
#include <SFML/Graphics.hpp>
#include <utility>
#include <memory>
#include <chrono>
#include <EvoAI/NeuralNetwork.hpp>
#include <EvoAI/Utils.hpp>
#include <EvoAI/Genome.hpp>
#include "GRenderer.hpp"

int main(int argc, char **argv){
    using EvoAI::NeuralNetwork;
    using EvoAI::Connection;
    using EvoAI::GRenderer;
    using EvoAI::Genome;
    bool running = true;
    if(argc < 2){
        std::cout << "usage:\n";
        std::cout << argv[0] << " <filename>" << std::endl;
        return EXIT_FAILURE;
    }
    // build Genome
    std::unique_ptr<Genome> g = std::make_unique<Genome>(std::string(argv[1]));
    // build renderer for the g.
    std::unique_ptr<GRenderer> gr = std::make_unique<GRenderer>(g.get(), sf::Vector2f(10,10));
    sf::RenderWindow App(sf::VideoMode(1270, 720), "Genome Visualizer - " + std::string(argv[1]));
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
                    break;
                default:
                    break;
            }
        }
        App.clear(sf::Color::Black);
        gr->Render(App);
        App.display();
    }
    App.close();
    return 0;
}
