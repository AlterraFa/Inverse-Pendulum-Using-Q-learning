#include "events.hpp"
#include "config.hpp"

void processEvents(sf::RenderWindow& window){
    sf::Event event;

    // Correct usage of pollEvent()
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
    }
}