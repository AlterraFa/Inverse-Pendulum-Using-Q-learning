#include "events.hpp"

#define UP      1
#define DOWN    2
#define LEFT    3
#define RIGHT   4

void processEvents(sf::RenderWindow& window, int& inputType){
    sf::Event event;

    // Correct usage of pollEvent()
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
        else if (event.type == sf::Event::KeyPressed){
            switch (event.key.code){
                case sf::Keyboard::Left:
                    inputType = LEFT;
                    break;
                case sf::Keyboard::Right:
                    inputType = RIGHT;
                    break;
                case sf::Keyboard::Up:
                    inputType = UP;
                    break;
                case sf::Keyboard::Down:
                    inputType = DOWN;
                    break;
            }
        }
        else if (event.key.code == sf::Keyboard::Escape) window.close();
        else inputType = 0;
    }
}