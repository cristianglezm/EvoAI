#include <iostream>
#include <SFML/Graphics.hpp>
#include <utility>
#include <memory>
#include <chrono>
#include <EvoAI/NeuralNetwork.hpp>
#include <EvoAI/Utils.hpp>
#include "NNRenderer.hpp"

void usage(){
    std::cout << "usage:\n";
    std::cout << "NeuralNetworkVisualizer" << " <filename> --dot" << std::endl;
}

int main(int argc, char **argv){
    using EvoAI::NeuralNetwork;
    using EvoAI::Connection;
    using EvoAI::NNRenderer;
    bool running = true;
    bool showText = false;
    std::string filename = "";
    bool saveDotFile = false;
    if(argc < 2){
        usage();
        return EXIT_FAILURE;
    }
    for(auto i=0;i<argc;++i){
        std::string opt(argv[i]);
        if(opt == "--dot"){
            saveDotFile = true;
        }else if(!opt.empty()){
            filename = opt;
        }
    }
    if(filename.empty()){
        usage();
        std::cout << "filename is empty." << std::endl;
        return 1;
    }
    // build network
    std::unique_ptr<NeuralNetwork> nn = std::make_unique<NeuralNetwork>(filename);
    if(saveDotFile){
        std::cout << "writing dot file" << std::endl;
        nn->writeDotFile(filename.substr(0,filename.find_first_of('.')) + ".dot");
    }
    // build renderer for the nn.
    std::unique_ptr<NNRenderer> nr = std::make_unique<NNRenderer>(nn.get());
    sf::RenderWindow App(sf::VideoMode(1270, 720), "Neural Network Visualizer - " + filename);
    auto counter = 2u;
    while(running){
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
                        ++counter;
                    }
                    if(event.key.code == sf::Keyboard::Subtract){
                        sf::View v = App.getView();
                        v.zoom(1.5);
                        App.setView(v);
                        ++counter;
                    }
                    break;
                case sf::Event::KeyPressed:
                    if(event.key.code == sf::Keyboard::Down){
                        auto v = App.getView();
                        v.move(0,100);
                        App.setView(v);
                        ++counter;
                    }
                    if(event.key.code == sf::Keyboard::Up){
                        auto v = App.getView();
                        v.move(0,-100);
                        App.setView(v);
                        ++counter;
                    }
                    if(event.key.code == sf::Keyboard::Right){
                        auto v = App.getView();
                        v.move(100,0);
                        App.setView(v);
                        ++counter;
                    }
                    if(event.key.code == sf::Keyboard::Left){
                        auto v = App.getView();
                        v.move(-100,0);
                        App.setView(v);
                        ++counter;
                    }
                    if(event.key.code == sf::Keyboard::T){
                        showText = !showText;
                        ++counter;
                    }
                    break;
                default:
                    break;
            }
        }
        if(counter > 0u){
            App.clear(sf::Color::Black);
            nr->Render(App,showText);
            App.display();
            --counter;
        }
    }
    App.close();
    return 0;
}
