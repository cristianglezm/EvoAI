#include <iostream>
#include <SFML/Graphics.hpp>

int main(){
    sf::RenderWindow App(sf::VideoMode(800, 600), "Neural Network Visualizer");

    while (App.isOpen()){
        sf::Event Event;
        while(App.pollEvent(Event)){
            if (Event.type == sf::Event::Closed){
                App.close();
            }
        }
        App.clear(sf::Color::Black);

        App.display();
    }
    return 0;
}
