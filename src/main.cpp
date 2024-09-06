#include "events.hpp"
#include "config.hpp"
#include "layout.hpp"
#include "obj.hpp"
#include <cmath>


std::vector<float> funct(std::vector<float> x, float accel){
    float xdot = x[1];
    float xddot = accel;
    return {xdot, xddot};
}

void forceRungeKutta(std::vector<float>& x, float accel, float timeStep){
    std::vector<float> f1 = funct(x, accel);

    std::vector<float> x2 = {x[0] + timeStep * f1[0] * .5f, x[1] + timeStep * f1[1] * .5f};
    std::vector<float> f2 = funct(x2, accel);

    std::vector<float> x3 = {x[0] + timeStep * f2[0] * .5f, x[1] + timeStep * f2[1] * .5f};
    std::vector<float> f3 = funct(x3, accel);

    std::vector<float> x4 = {x[0] + timeStep * f3[0], x[1] + timeStep * f3[1]};
    std::vector<float> f4 = funct(x4, accel);

    std::cout << f1[1] << " " << f2[1] << " " << f3[1] << " " << f4[1] << '\n';
    x[0] += (1.f/6.f) * timeStep * (f1[0] + 2 * f2[0] + 2 * f3[0] + f4[0]);
    x[1] += (1.f/6.f) * timeStep * (f1[1] + 2 * f2[1] + 2 * f3[1] + f4[1]);
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
        std::vector<float> x = {cartPosition.x, cartVel};
        forceRungeKutta(x, a, conf::timeStep);

        
        cartPosition.x = x[0];
        cartVel = x[1];

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
