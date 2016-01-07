#include <iostream>
#include <SFML/Graphics.hpp>
#include <EvoAI/NeuralNetwork.hpp>
#include <EvoAI/NNRenderer.hpp>
#include <utility>
#include <memory>
#include <EvoAI/Utils.hpp>
#include <chrono>

int main(int argc, char **argv){
    using EvoAI::NeuralNetwork;
    using EvoAI::Connection;
    using EvoAI::NNRenderer;
    bool running = true;
    std::unique_ptr<NeuralNetwork> nn = EvoAI::createElmanNeuralNetwork(1,2,2,1,1.0);
    std::vector<double> inputs;
    inputs.emplace_back(0.0);
    //nn->setInputs(std::move(inputs));
    auto start = std::chrono::high_resolution_clock::now();
    auto res = nn->run();
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Running NN took " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us.\n";
    
    start = std::chrono::high_resolution_clock::now();
    std::unique_ptr<NNRenderer> nr = std::make_unique<NNRenderer>(nn.get());
    end = std::chrono::high_resolution_clock::now();
    std::cout << "NNRenderer constructor took " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us.\n";
    
    std::cout << "output: " << res[0] << std::endl;
    sf::RenderWindow App(sf::VideoMode(1270, 720), "Neural Network Visualizer");
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
        nr->Render(App,true);
        App.display();
    }
    App.close();
    return 0;
}
