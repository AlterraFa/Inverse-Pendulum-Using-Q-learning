#include "events.hpp"
#include "config.hpp"

void processEvents(std::unique_ptr<sf::RenderWindow>& window){
    sf::Event event;
    while (window -> pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window -> close();
    }
}
