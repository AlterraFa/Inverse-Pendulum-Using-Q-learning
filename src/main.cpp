#include "events.hpp"
#include "config.hpp"
#include "layout.hpp"
#include "obj.hpp"
#include <cmath>


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


    Rectangle cart(150, 50);
    float cartMass = 10;
    float inputForce = 100;
    sf::Vector2f cartPosition = sf::Vector2f(800, 530);
    float cartVel = 0;

    float theta = 0;
    float length = 300;
    Circle pendulum(20);
    sf::Vector2f pendulumPosition;
    Circle point(5);


    int inputType;
    while (window.isOpen()) {
        processEvents(window, inputType);        

                 
        float a = (inputType == RIGHT)? inputForce / cartMass: (inputType == LEFT)? -inputForce / cartMass: 0;
        force(cartPosition.x, cartVel, a, conf::timeStep);
        if (cartPosition.x<= std::get<0>(conf::railBound)){
            cartVel = 0;
            cartPosition.x = std::get<0>(conf::railBound);
        } else if (cartPosition.x >= std::get<1>(conf::railBound)){
            cartVel = 0;
            cartPosition.x = std::get<1>(conf::railBound);
        }
        cart.setPosition(cartPosition);
        sf::Vector2f cartPosition = cart.getPosition();

        theta += 0.1 * conf::timeStep;
        pendulumPosition.x = cartPosition.x - length * sinf(theta);
        pendulumPosition.y = cartPosition.y - length * cosf(theta);
        pendulum.setPosition(pendulumPosition);


        point.setPosition(cartPosition);        
        window.clear(sf::Color(50, 50, 50));
        window.draw(cart);
        window.draw(upperRail);
        window.draw(lowerRail);
        window.draw(rightCircle);
        window.draw(leftCircle);
        window.draw(pendulum);
        window.draw(point);
        window.display();
    }

    return 0;
}
