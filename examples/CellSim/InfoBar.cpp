#include "InfoBar.hpp"

namespace EvoAI{
    InfoBar::InfoBar(float width, float height)
    : background(sf::Vector2f(width, height))
    , info(sf::Vector2f(width, height)){}
    void InfoBar::setPosition(float x, float y) noexcept{
        background.setPosition(x,y);
        info.setPosition(x,y);
    }
    void InfoBar::setPosition(sf::Vector2f pos) noexcept{
        background.setPosition(pos);
        info.setPosition(pos);
    }
    void InfoBar::setBackgroundFillColor(sf::Color c) noexcept{
        background.setFillColor(c);
    }
    void InfoBar::setFillColor(sf::Color c) noexcept{
        info.setFillColor(c);
    }
    void InfoBar::setOutlineColor(sf::Color c) noexcept{
        info.setOutlineColor(c);
    }
    void InfoBar::adjustAlphaChannel(sf::Uint8 value) noexcept{
        auto bc = background.getFillColor();
        auto ic = info.getFillColor();
        bc.a = value;
        ic.a = value;
        background.setFillColor(bc);
        info.setFillColor(ic);
    }
    void InfoBar::updateInfo(float value) noexcept{
        auto size = info.getSize();
        info.setSize(sf::Vector2f(value, size.y));
    }
    void InfoBar::render(sf::RenderWindow& win) noexcept{
        win.draw(background);
        win.draw(info);
    }
    sf::Vector2f InfoBar::getSize() const noexcept{
        return background.getSize();
    }
}