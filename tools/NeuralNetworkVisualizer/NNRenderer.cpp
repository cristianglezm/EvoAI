#include "NNRenderer.hpp"

namespace EvoAI{
    NNRenderer::NNRenderer(NeuralNetwork* nn)
    : neurons()
    , connections()
    , nn(nn){
        font = sf::Font();
        if(!font.loadFromFile("data/fonts/Outwrite.ttf")){
            throw std::runtime_error("Cannot Load Font -> data/fonts/Outwrite.ttf");
        }
        setUp();
    }
    void NNRenderer::Render(sf::RenderWindow& win, bool renderTexts) noexcept{
        for(auto& ci:connections){
            if(renderTexts){
                win.draw(ci.info);
            }
            win.draw(ci.connectionLine, 2, sf::Lines);
        }
        for(auto& ni:neurons){
            if(renderTexts){
                win.draw(ni.info);
            }
            win.draw(ni.neuronShape);
        }
    }
    void NNRenderer::setNeuralNetwork(NeuralNetwork* nn) noexcept{
        neurons.clear();
        connections.clear();
        this->nn = nn;
        setUp();
    }
    void NNRenderer::setFont(sf::Font f) noexcept{
        font = f;
    }
/// private
    void NNRenderer::setUp() noexcept{
        sf::Vector2f position(5.0,5.0);
        sf::Vector2f space(0.0,150.0);
        for(auto& l: nn->getLayers()){
            for(auto& neuron:l.getNeurons()){
                setUpNeuronInfo(neuron,position);
                position += space;
            }
            position = sf::Vector2f(position.x,5.0);
            position += sf::Vector2f(270.0,0.0);
        }
    }
    void NNRenderer::setUpNeuronInfo(Neuron& n, sf::Vector2f& p) noexcept{
        NeuronInfo ni;
        const int textSize = 15;
        ni.info = sf::Text(n.toString("\n"), font, textSize);
        ni.info.setPosition(sf::Vector2f(p.x+5.0,p.y+10));
        ni.neuronShape = sf::CircleShape(6);
        ni.neuronShape.setPosition(p);
        ni.neuronShape.setFillColor(sf::Color::Red);
        neurons.emplace_back(ni);
        int space = 10;
        bool change = false;
        for(auto& c:n.getConnections()){
            if(change){
                setUpConnectionInfo(c,p, space);
            }else{
                setUpConnectionInfo(c,p, -space);
            }
            change = !change;
        }
    }
    void NNRenderer::setUpConnectionInfo(Connection& c, sf::Vector2f& p, const int& space) noexcept{
        const int textSize = 15;
        ConnectionInfo ci;
        sf::Color color = sf::Color::White;
        if(c.isRecurrent()){
            color = sf::Color::Red;
        }
        ci.connectionLine[0] = sf::Vertex(sf::Vector2f(p.x+5,p.y+5),color);
        auto dlyr = c.getDest().layer * 270.0 + 10;
        auto dnrn = c.getDest().neuron * 150.0 + 10;
        ci.connectionLine[1] = sf::Vertex(sf::Vector2f(dlyr,dnrn),color);
        ci.info = sf::Text("W: " + std::to_string(c.getWeight()),font, textSize);
        ci.info.setPosition(sf::Vector2f(((p.x + dlyr) / 2),(p.y + dnrn) / 2 - space));
        connections.emplace_back(ci);
    }
}

