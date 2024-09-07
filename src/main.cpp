#include "events.hpp"
#include "config.hpp"
#include "layout.hpp"
#include "obj.hpp"
#include <cmath>

std::vector<float> funct(std::vector<float> x, float M, float m, float length, float force, float forceS){
    float xdot = x[1];
    float thetadot = x[3];

    float a = M + m;
    float b = m * length * cosf(x[2]);
    float c = force - m * length * powf(thetadot, 2) * sinf(x[2]);
    float d = length;
    float e = cosf(x[2]);
    float f = conf::cgravity * sinf(x[2]) + forceS;

    float xddot = ((c / a) + ((b * f) / (a * d))) / (1 - ((b * e) / (a * d)));
    float thetaddot = (f + e * xddot) / d;

    return {xdot, xddot, thetadot, thetaddot};
}

std::vector<float> vectorPushback(std::vector<float> x, std::vector<float> funct, float timeStep, float coeff){
    std::vector<float> res;
    for (size_t i = 0; i < x.size(); i++){
        res.push_back(x[i] + coeff * timeStep * funct[i]);
    }
    return res;
}

void stateUpdate(std::vector<float>& x, float M, float m, float length, float force, float forceS, float timeStep){

    std::vector<float> f1 = funct(x, M, m, length, force, forceS);

    std::vector<float> x2 = vectorPushback(x, f1, timeStep, .5f);
    std::vector<float> f2 = funct(x2, M, m, length, force, forceS);

    std::vector<float> x3 = vectorPushback(x, f2, timeStep, .5f);
    std::vector<float> f3 = funct(x3, M, m, length, force, forceS);

    std::vector<float> x4 = vectorPushback(x, f3, timeStep, 1.f);
    std::vector<float> f4 = funct(x4, M, m, length, force, forceS);

    for (size_t i = 0; i < f1.size(); i ++){
        x[i] += (1.f / 6.f) * timeStep * (f1[i] + 2 * f2[i] + 2 * f3[i] + f4[i]);
    }
}



signed main() {


    // For esthetic only
    sf::RenderWindow window = conf::createWindow();
    sf::RectangleShape lowerRail, upperRail;
    std::tie(lowerRail, upperRail) = createRail(290, 20, 290, 1270);

    sf::VertexArray rightCircle, leftCircle;
    std::tie(leftCircle, rightCircle) = cap(12.5f, 7.5f, std::make_tuple(925, 292.5), 1270);

    Border border(sf::Vector2f(100, 50), sf::Vector2f(1650, 500), 10.5f, sf::Color(232, 109, 80), 5.f);
    // For esthetic only

    float inputForce = 350;

    Rectangle cart(150, 20);
    float cartMass = 50;
    float cartVel = 0;
    cart.setPosition(sf::Vector2f(900, 290));


    Circle pendulum(20);
    Circle pendulumRim(23);
    pendulumRim.overrideColor(sf::Color::White);
    float pendulumMass = 10.5;
    float angularVel = 0;
    float theta = M_PI;
    float length = 200;
    
    Circle pivot(20);
    Circle pivotRim(23);
    pivotRim.overrideColor(sf::Color::White);

    Rectangle rod(length, 6); 

    sf::Vector2f newCartPos = cart.getPosition(), newPendulumPos, newRodPos;
    int inputType;
    while (window.isOpen()) {
        processEvents(window, inputType);        

        float force = (inputType == RIGHT && !(newCartPos.x >= std::get<1>(conf::railBound)))? inputForce: (inputType == LEFT && !(newCartPos.x <= std::get<0>(conf::railBound)))? -inputForce: 0;
        float friction = -((cartVel > 0) - (cartVel < 0)) * 5;
        float angularFriction = -((angularVel > 0) - (angularVel < 0)) * .095;

        std::vector<float> stateVar = {cart.getPosition().x, cartVel, theta, angularVel};
        stateUpdate(stateVar, cartMass, pendulumMass, length, force + friction, angularFriction, conf::timeStep);

        newCartPos = sf::Vector2f(stateVar[0], cart.getPosition().y);
        cartVel = stateVar[1];
        theta = stateVar[2];
        angularVel = stateVar[3];
        newPendulumPos = sf::Vector2f(newCartPos.x - length * sinf(theta), newCartPos.y - length * cosf(theta));

        std::cout << cartVel << "\n";

        if (newCartPos.x <= std::get<0>(conf::railBound)){
            cartVel = 0;
            newCartPos.x = std::get<0>(conf::railBound);
        }
        else if (newCartPos.x >= std::get<1>(conf::railBound)){
            cartVel = 0;
            newCartPos.x = std::get<1>(conf::railBound);
        }

        newRodPos.x = (newCartPos.x + newPendulumPos.x) / 2;
        newRodPos.y = (newCartPos.y + newPendulumPos.y) / 2;

        // Update position
        cart.setPosition(newCartPos);        
        pendulum.setPosition(newPendulumPos);
        pivot.setPosition(newCartPos);
        pendulumRim.setPosition(newPendulumPos);
        pivotRim.setPosition(newCartPos);
        rod.setPosition(newRodPos);
        rod.setRotation(-theta * 180 / M_PI + 90);


        window.clear(sf::Color(50, 50, 50));
        window.draw(border);
        window.draw(cart);
        window.draw(upperRail);
        window.draw(lowerRail);
        window.draw(rightCircle);
        window.draw(leftCircle);
        window.draw(rod);
        window.draw(pendulumRim);
        window.draw(pendulum);
        window.draw(pivotRim);
        window.draw(pivot);
        window.display();
    }

    return 0;
}
