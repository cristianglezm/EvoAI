#ifndef EVOAI_INFO_BAR_HPP
#define EVOAI_INFO_BAR_HPP

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

namespace EvoAI{
    /**
     * @brief Info Bar health, stamina and nutrients bar,
     */
    class InfoBar{
        public:
            /**
             * @brief constructor
             * @param width 
             * @param height 
             */
            InfoBar(float width = 100.0, float height = 15.0);
            /**
             * @brief sets position
             * @param x 
             * @param y 
             */
            void setPosition(float x, float y) noexcept;
            /**
             * @brief sets position
             * @param pos 
             */
            void setPosition(sf::Vector2f pos) noexcept;
            /**
             * @brief sets sf::Color of the background
             * @param c 
             */
            void setBackgroundFillColor(sf::Color c) noexcept;
            /**
             * @brief sets sf::Color of the info
             * @param c 
             */
            void setFillColor(sf::Color c) noexcept;
            /**
             * @brief sets the outlinee sf::Color of info.
             * @param c 
             */
            void setOutlineColor(sf::Color c) noexcept;
            /**
             * @brief sets alpha channel of background and info.
             * @param value 
             */
            void adjustAlphaChannel(sf::Uint8 value) noexcept;
            /**
             * @brief updates the info value.
             * @param value 
             */
            void updateInfo(float value) noexcept;
            /**
             * @brief render info bar
             * @param win 
             */
            void render(sf::RenderWindow& win) noexcept;
            /**
             * @brief gets width and height
             * @return 
             */
            sf::Vector2f getSize() const noexcept;
        private:
            sf::RectangleShape background;
            sf::RectangleShape info;
    };
}

#endif // EVOAI_INFO_BAR_HPP
