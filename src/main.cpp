#include <SFML/Graphics.hpp>
#include "events.hpp"
#include "config.hpp"
#include "layout.hpp"
#include "obj.hpp"


void force(float& position, float& velocity, float acceleration, float t) {
    velocity += acceleration * t;
    position += velocity * t;
}

void resetPosition(float& positionX, float& positionY, std::tuple<int, int> boundary){
    positionX = (positionX >= std::get<0>(boundary))? 0: positionX;
    positionY = (positionY >= std::get<1>(boundary))? 0: positionY;
}

int main() {


    // For esthetic only
    sf::RenderWindow window = conf::createWindow();
    sf::RectangleShape lowerRail, upperRail;
    std::tie(lowerRail, upperRail) = createRail(530, 20, 250, 1350);

    sf::VertexArray rightCircle, leftCircle;
    std::tie(leftCircle, rightCircle) = cap(12.5f, 7.5f, std::make_tuple(925, 532.5), 1350);
    // For esthetic only


    Rectangle cart(150, 50, 700, 505);
    float cartMass = 10;
    float inputForce = 100;
    float x, y;
    float velocity = 0;


    int inputType;
    while (window.isOpen()) {
        processEvents(window, inputType);        
        std::vector<sf::Vector2f> corners = cart.getCornerPositions();


        std::tie(x, y) = cart.getPosition();            
        float a = (inputType == RIGHT)? inputForce / cartMass: (inputType == LEFT)? -inputForce / cartMass: 0;
        force(x, velocity, a, conf::timeStep);
        if (x + 700 / 2 <= std::get<0>(conf::railBound)){
            velocity = 0;
            x = std::get<0>(conf::railBound) - 700 / 2;
        } else if (x + 700 / 2 >= std::get<1>(conf::railBound)){
            velocity = 0;
            x = std::get<1>(conf::railBound) - 700 / 2;
        }
        cart.setPosition(x, y);

        std::cout << x << " " << std::get<1>(conf::railBound) << "\n";


        window.clear(sf::Color(50, 50, 50));
        window.draw(cart);
        window.draw(upperRail);
        window.draw(lowerRail);
        window.draw(rightCircle);
        window.draw(leftCircle);
        window.display();
    }

    return 0;
}
