#include "events.hpp"
#include "config.hpp"
#include "layout.hpp"
#include "obj.hpp"


signed main() {


    // For esthetic only
    sf::RenderWindow window = conf::createWindow(conf::getSettings());
    sf::RectangleShape lowerRail, upperRail;
    std::tie(lowerRail, upperRail) = createRail(290, 20, 290, 1270);

    sf::VertexArray rightCircle, leftCircle;
    std::tie(leftCircle, rightCircle) = cap(12.5f, 7.5f, std::make_tuple(925, 292.5), 1270);

    Border pendulumbBorder(sf::Vector2f(100, 50), sf::Vector2f(1650, 500), 10.5f, sf::Color(232, 109, 80), 15.f);
    Border velGraphBorder(sf::Vector2f(100, 650), sf::Vector2f(600, 300), 5.f, sf::Color(219, 192, 118), 12.5f);
    Border angularGraphBorder(sf::Vector2f(750, 650), sf::Vector2f(600, 300), 5.f, sf::Color(219, 192, 118), 12.5f);
    // For esthetic only

    int inputType;
    float cartForce = 350, pendulumForce = 30;

    float cartMass = 50;
    float pendulumMass = 10.5;
    float angularVel = 0;
    float theta = M_PI;
    float armLength = 200;
    float cartLinearVelocity, pendulumAngularVelocity;
    sf::Vector2f cartPosition(900, 290);
    Pendulum pendulum(cartMass, pendulumMass, armLength, cartPosition);

    Rectangle test(150, 20);
    test.setPosition(cartPosition);


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

        std::cout << inputType << "\n";      

        std::tie(cartPosition, cartLinearVelocity, pendulumAngularVelocity) = pendulum.stateUpdate(cartForce, 
                                                                                                   pendulumForce,  
                                                                                                   conf::timeStep,
                                                                                                   inputType, conf::railBound);


        cartGrapher.update(cartLinearVelocity, 2);
        pendulumGrapher.update(pendulumAngularVelocity * 180 / M_PI, 2);



        window.clear(sf::Color(50, 50, 50));
        window.draw(velGraphBorder);
        window.draw(pendulumbBorder);
        window.draw(angularGraphBorder);
        window.draw(upperRail);
        window.draw(lowerRail);
        window.draw(pendulum);
        window.draw(rightCircle);
        window.draw(leftCircle);
        window.draw(cartGrapher);
        window.draw(pendulumGrapher);
        window.display();
    }

    return 0;
}
