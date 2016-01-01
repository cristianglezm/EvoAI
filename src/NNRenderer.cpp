#include <EvoAI/NNRenderer.hpp>

namespace EvoAI{
    NNRenderer::NNRenderer(NeuralNetwork* nn)
    : neurons()
    , connections()
    , nn(nn){
        font = sf::Font();
        font.loadFromFile("data/Outwrite.ttf");
        setUp();
    }
    void NNRenderer::Render(sf::RenderWindow& win, bool renderTexts){
        for(auto& ci:connections){
            if(renderTexts){
                win.draw(ci.weight);
                win.draw(ci.cycles);
                win.draw(ci.visited);
            }
            win.draw(ci.connectionLine, 2, sf::Lines);
        }
        for(auto& ni:neurons){
            if(renderTexts){
                win.draw(ni.value);
                win.draw(ni.oldValue);
                win.draw(ni.biasWeight);
                win.draw(ni.error);
                win.draw(ni.type);
            }
            win.draw(ni.neuronShape);
        }
    }
/// private
    void NNRenderer::setUp(){
        sf::Vector2f position(5.0,5.0);
        sf::Vector2f space(0.0,150.0);
        for(auto& l: nn->layers){
            for(auto& neuron:l.getNeurons()){
                setUpNeuronInfo(neuron,position);
                position += space;
            }
            position = sf::Vector2f(position.x,5.0);
            position += sf::Vector2f(270.0,0.0);
        }
    }
    void NNRenderer::setUpNeuronInfo(Neuron& n, sf::Vector2f& p){
        NeuronInfo ni;
        const int textSize = 15;
        ni.value = sf::Text("Val: " + std::to_string(n.getValue()), font, textSize);
        ni.value.setPosition(sf::Vector2f(p.x+5.0,p.y+10));
        ni.oldValue = sf::Text("OldVal: " + std::to_string(n.getOldValue()),font, textSize);
        ni.oldValue.setPosition(sf::Vector2f(p.x+5.0,p.y+30));
        ni.biasWeight = sf::Text("bWeight: " + std::to_string(n.getBiasWeight()),font,textSize);
        ni.biasWeight.setPosition(sf::Vector2f(p.x+5.0,p.y+50));
        ni.error = sf::Text("Err: " + std::to_string(n.getError()),font,textSize);
        ni.error.setPosition(sf::Vector2f(p.x+5.0,p.y+70));
        switch(n.getType()){
            case Neuron::Type::CONTEXT:
                    ni.type = sf::Text("type: Context",font,textSize);
                    ni.type.setPosition(sf::Vector2f(p.x+5.0,p.y+90));
                break;
            case Neuron::Type::HIDDEN:
                    ni.type = sf::Text("type: Hidden",font,textSize);
                    ni.type.setPosition(sf::Vector2f(p.x+5.0,p.y+90));
                break;
            case Neuron::Type::INPUT:
                    ni.type = sf::Text("type: Input",font,textSize);
                    ni.type.setPosition(sf::Vector2f(p.x+5.0,p.y+90));
                break;
            case Neuron::Type::OUTPUT:
                    ni.type = sf::Text("type: Output",font,textSize);
                    ni.type.setPosition(sf::Vector2f(p.x+5.0,p.y+90));
                break;
            default:
                    ni.type = sf::Text("type: Undefined",font,textSize);
                    ni.type.setPosition(sf::Vector2f(p.x+5.0,p.y+90));
                break;
        }
        ni.neuronShape = sf::CircleShape(6);
        ni.neuronShape.setPosition(p);
        ni.neuronShape.setFillColor(sf::Color::Red);
        neurons.push_back(ni);
        for(auto& c:n.getConnections()){
            auto textPos = sf::Vector2f(p.x,p.y+10);
            setUpConnectionInfo(c,textPos);
        }
    }
    void NNRenderer::setUpConnectionInfo(Connection& c, sf::Vector2f& p){
        const int textSize = 15;
        ConnectionInfo ci;
        sf::Color color = sf::Color::White;
        if(c.isRecurrent()){
            color = sf::Color::Red;
        }
        ci.connectionLine[0] = sf::Vertex(sf::Vector2f(p.x+5,p.y-5),color);
        auto dlyr = c.getDest().layer * 270.0 + 8.0;
        auto dnrn = c.getDest().neuron * 150.0 + 8.0;
        ci.connectionLine[1] = sf::Vertex(sf::Vector2f(dlyr,dnrn),color);
        ci.weight = sf::Text("Weight: " + std::to_string(c.getWeight()),font, textSize);
        ci.weight.setPosition(sf::Vector2f(p.x+150,p.y+15));
        ci.cycles = sf::Text("Cycles: " + std::to_string(c.getCycles()),font, textSize);
        ci.cycles.setPosition(sf::Vector2f(p.x+150,p.y+35));
        std::string isVisited = "";
        if(c.isVisited()){
            isVisited = "True";
        }else{
            isVisited = "False";
        }
        ci.visited = sf::Text("Visited: " + isVisited,font, textSize);
        ci.visited.setPosition(sf::Vector2f(p.x+150,p.y+55));
        connections.push_back(ci);
    }
}

