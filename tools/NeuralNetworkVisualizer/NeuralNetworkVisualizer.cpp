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
    bool running = true;
    std::unique_ptr<NeuralNetwork> nn = EvoAI::createElmanNeuralNetwork(1,2,2,1,1.0);
    //(*nn)[2].setActivationType(EvoAI::NeuronLayer::TANH);
    //(*nn)[3].setActivationType(EvoAI::NeuronLayer::SINUSOID);
    std::vector<double> inputs;
    inputs.emplace_back(0.0);
    /// TODO mirar formula revisar
    nn->addConnection(Connection({2,1},{2,0},-0.5));
    nn->setInputs(std::move(inputs));
    auto res = nn->run();
    std::unique_ptr<NNRenderer> nr = std::make_unique<NNRenderer>(nn.get());
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
