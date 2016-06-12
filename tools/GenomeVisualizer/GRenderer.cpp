#include "GRenderer.hpp"

namespace EvoAI{
    GRenderer::GRenderer(Genome* g)
    : genome(g)
    , gRect()
    , font(){
        if(!font.loadFromFile("data/fonts/Times_New_Roman_Normal.ttf")){
            throw std::runtime_error("Cannot Load Font -> data/fonts/Times_New_Roman_Normal.ttf");
        }
    }
    void GRenderer::Render(sf::RenderWindow& win,bool renderTexts = false) noexcept{
        
    }
    void GRenderer::setUpInfo(){
        
    }
}