#ifndef EVOAI_GRENDERER_HPP
#define EVOAI_GRENDERER_HPP

#include <EvoAI/Genome.hpp>
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <vector>

namespace EvoAI{
    class GRenderer{
        public:
            GRenderer(Genome* g);
            void Render(sf::RenderWindow& win,bool renderTexts = false) noexcept;
            ~GRenderer() = default;
        private:
            struct GenomeInfo{
                sf::Text info;
                sf::RectangleShape rect;
            }
        private:
            void setUpInfo();
        private:
            Genome genome;
            sf::RectangleShape gRect;
            sf::Font font;
    };
}

#endif // EVOAI_GRENDERER_HPP