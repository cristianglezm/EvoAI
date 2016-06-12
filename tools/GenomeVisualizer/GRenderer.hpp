#ifndef EVOAI_GRENDERER_HPP
#define EVOAI_GRENDERER_HPP

#include <EvoAI/Genome.hpp>
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <vector>

namespace EvoAI{
    class GRenderer{
        public:
            GRenderer(Genome* g, const sf::Vector2f& position);
            void Render(sf::RenderWindow& win) noexcept;
            ~GRenderer() = default;
        private:
            struct GenomeInfo{
                sf::Text info;
                sf::RectangleShape rect;
            };
        private:
            void setUpInfo();
        private:
            Genome* genome;
            sf::RectangleShape gRect;
            sf::Font font;
            std::vector<GenomeInfo> gInfos;
    };
}

#endif // EVOAI_GRENDERER_HPP