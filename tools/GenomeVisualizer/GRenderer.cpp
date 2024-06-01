#include "GRenderer.hpp"

namespace EvoAI{
    GRenderer::GRenderer(Genome* g, const sf::Vector2f& position)
    : genome(g)
    , gRect()
    , font()
    , gInfos(){
        if(!font.loadFromFile("EvoAI/data/fonts/MonaspaceRadon-Regular.otf")){
            throw std::runtime_error("Cannot Load Font -> EvoAI/data/fonts/MonaspaceRadon-Regular.otf");
        }
        gRect.setPosition(position);
        gRect.setFillColor(sf::Color::Transparent);
        gRect.setOutlineThickness(5);
        gRect.setOutlineColor(sf::Color::White);
        gRect.setSize(sf::Vector2f(200 * (g->getConnectionChromosomes().size()),110));
        setUpInfo();
    }
    void GRenderer::Render(sf::RenderWindow& win) noexcept{
        for(auto& gi:gInfos){
            win.draw(gi.info);
            win.draw(gi.rect);
        }
        win.draw(gRect);
    }
    void GRenderer::setUpInfo(){
        sf::Vector2f space(200.0,0.0);
        auto currentPos = gRect.getPosition() + sf::Vector2f(1,1);
        for(auto& cg:genome->getConnectionChromosomes()){
            GenomeInfo gi;
            gi.info.setFont(font);
            gi.info.setCharacterSize(13u);
            gi.info.setString(cg.toString("\n"));
            gi.info.setPosition(currentPos + sf::Vector2f(5,5));
            gi.rect.setSize(sf::Vector2f(200,110));
            gi.rect.setPosition(currentPos);
            gi.rect.setFillColor(sf::Color::Transparent);
            gi.rect.setOutlineThickness(2);
            gi.rect.setOutlineColor(sf::Color::White);
            gInfos.push_back(gi);
            currentPos += space;
        }
    }
}