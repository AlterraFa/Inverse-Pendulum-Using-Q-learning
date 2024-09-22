#include "events.hpp"
#include "config.hpp"
#include <SFML/Window/Keyboard.hpp>

void processEvents(std::unique_ptr<sf::RenderWindow>& window, int& inputType){
    sf::Event event;
    while (window -> pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window -> close();
        if (event.type == sf::Event::KeyPressed){
            switch (event.key.code){
                case sf::Keyboard::Up:
                    inputType = UP;
                    break;
                case sf::Keyboard::Down:
                    inputType = DOWN;
                    break;
                case sf::Keyboard::Left:
                    inputType = LEFT;
                    break;
                case sf::Keyboard::Right:
                    inputType = RIGHT;
                    break;
                case sf::Keyboard::Escape:
                    inputType = EXIT;
                    window -> close();
                    break;
            }
        }
        else inputType = 0;
    }
}
