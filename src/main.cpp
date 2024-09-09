#include "events.hpp"
#include "config.hpp"
#include "layout.hpp"
#include "obj.hpp"
#include "physics.hpp"


signed main() {


    // For esthetic only
    sf::RenderWindow window = conf::createWindow();
    sf::RectangleShape lowerRail, upperRail;
    std::tie(lowerRail, upperRail) = createRail(290, 20, 290, 1270);

    sf::VertexArray rightCircle, leftCircle;
    std::tie(leftCircle, rightCircle) = cap(12.5f, 7.5f, std::make_tuple(925, 292.5), 1270);

    Border pendulumbBorder(sf::Vector2f(100, 50), sf::Vector2f(1650, 500), 10.5f, sf::Color(232, 109, 80), 15.f);
    Border velGraphBorder(sf::Vector2f(100, 650), sf::Vector2f(600, 300), 5.f, sf::Color(219, 192, 118), 12.5f);
    Border angularGraphBorder(sf::Vector2f(750, 650), sf::Vector2f(600, 300), 5.f, sf::Color(219, 192, 118), 12.5f);
    // For esthetic only

    int inputType;
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

    sf::Vector2f newCartPos, newPendulumPos, newRodPos;


    size_t histSize = 4000, windowSize = 2;
    std::vector<sf::Vector2f> cartGraphBound = {sf::Vector2f(150, 700), sf::Vector2f(650, 900)};
    std::vector<sf::Vector2f> pendulumGraphBound = {sf::Vector2f(800, 700), sf::Vector2f(1300, 900)};

    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/JetBrainsMonoNerdFont-Light.ttf")) {
        std::cerr << "Error: could not load font" << "\n";
        return -21321432;
    }

    Graphing cartGrapher(histSize, font, cartGraphBound, 2.0f);
    Graphing pendulumGrapher(histSize, font, pendulumGraphBound, 2.0f);

    while (window.isOpen()) {
        processEvents(window, inputType);        

        float force = (inputType == RIGHT && !(newCartPos.x >= std::get<1>(conf::railBound)))? inputForce: (inputType == LEFT && !(newCartPos.x <= std::get<0>(conf::railBound)))? -inputForce: 0;
        float friction = -((cartVel > 0) - (cartVel < 0)) * 5;
        float angularFriction = -((angularVel > 0) - (angularVel < 0)) * .1;

        Eigen::Vector4f stateVar = {cart.getPosition().x, cartVel, theta, angularVel};
        stateUpdate(stateVar, cartMass, pendulumMass, length, force + friction, angularFriction, conf::timeStep);

        newCartPos = sf::Vector2f(stateVar[0], cart.getPosition().y);
        cartVel = stateVar[1];
        theta = stateVar[2];
        angularVel = stateVar[3];
        newPendulumPos = sf::Vector2f(newCartPos.x - length * sinf(theta), newCartPos.y - length * cosf(theta));


        // Collision with rail bound
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

        cartGrapher.update(cartVel, 2);
        pendulumGrapher.update(angularVel * 180 / M_PI, 2);

         


        // Update position
        cart.setPosition(newCartPos);        
        pendulum.setPosition(newPendulumPos);
        pivot.setPosition(newCartPos);
        pendulumRim.setPosition(newPendulumPos);
        pivotRim.setPosition(newCartPos);
        rod.setPosition(newRodPos);
        rod.setRotation(-theta * 180 / M_PI + 90);

        window.clear(sf::Color(50, 50, 50));
        window.draw(velGraphBorder);
        window.draw(pendulumbBorder);
        window.draw(angularGraphBorder);
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
        window.draw(cartGrapher);
        window.draw(pendulumGrapher);
        window.display();
    }

    return 0;
}
