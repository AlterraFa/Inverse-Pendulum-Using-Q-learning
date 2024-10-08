#include "events.hpp"
#include "config.hpp"
#include "layout.hpp"
#include "obj.hpp"
#include <sys/file.h>
#include <unistd.h>
#include <string>

signed main() {
    // For aesthetics only
    auto window = conf::createWindow(conf::getSettings());
    sf::RectangleShape lowerRail, upperRail;
    std::tie(lowerRail, upperRail) = createRail(290, 20, 290, 1270);

    sf::VertexArray rightCircle, leftCircle;
    std::tie(leftCircle, rightCircle) = cap(12.5f, 7.5f, std::make_tuple(925, 292.5), 1270);

    Border pendulumbBorder(sf::Vector2f(100, 50), sf::Vector2f(1650, 500), 10.5f, sf::Color(232, 109, 80), 15.f);
    Border velGraphBorder(sf::Vector2f(100, 650), sf::Vector2f(600, 300), 5.f, sf::Color(219, 192, 118), 12.5f);
    Border angularGraphBorder(sf::Vector2f(750, 650), sf::Vector2f(600, 300), 5.f, sf::Color(219, 192, 118), 12.5f);

    sf::Font font;
    if (!font.loadFromFile("/usr/local/share/fonts/JetBrainsMonoNerdFont-Light.ttf")) {
        std::cerr << "Error: could not load font" << "\n";
        return -21321432;
    }
    // For aesthetics only
    

    // Pendulum Initial Physics
    int inputType = 0;
    float pendulumForce = 30;
    std::vector<float> cartForce = {350.f, 200.f};

    float cartMass = 75;
    float pendulumMass = 15.5;
    float armLength = 200;
    float cartLinearVelocity, pendulumAngularVelocity, pendulumAttitude;
    sf::Vector2f cartPosition(900, 290);
    Pendulum pendulum(cartMass, pendulumMass, armLength, cartPosition);


    // Graphing Initialize
    size_t histSize = 4000;
    std::vector<sf::Vector2f> cartGraphBound = {sf::Vector2f(150, 700), sf::Vector2f(650, 900)};
    std::vector<sf::Vector2f> pendulumGraphBound = {sf::Vector2f(800, 700), sf::Vector2f(1300, 900)};

    bool endFlag = false;
    Graphing cartGrapher(histSize, font, cartGraphBound, 2.0f);
    Graphing pendulumGrapher(histSize, font, pendulumGraphBound, 2.0f);

    Eigen::VectorXd state(5);


    float neuralInput = 0.0f;

    while (inputType != EXIT) {
        processEvents(window, inputType);

        std::string input;
        if (!std::getline(std::cin, input)){
            std::cerr << "Cannot read input" << "\n";
            break;
        }
        try {
            neuralInput = std::stof(input);
        } catch (const std::invalid_argument& e){
            std::cerr << "Invalid input" << std::endl;
            continue;
        } catch (const std::out_of_range& e){
            std::cerr << "Input out of range" << std::endl;
            continue;
        }


        std::tie(cartPosition, cartLinearVelocity, pendulumAngularVelocity, pendulumAttitude) = pendulum.stateUpdate(
            cartForce, pendulumForce, conf::timeStep, neuralInput, conf::railBound, 12.5, 0.01);


        state << cartPosition.x, cartLinearVelocity, pendulumAngularVelocity, pendulumAttitude, endFlag;
        std::cout << state.format(Eigen::IOFormat(Eigen::StreamPrecision, Eigen::DontAlignCols, ", ", ", ", "", "", "")) << std::endl;
        std::cout.flush();

        float duration = cartGrapher.update(cartLinearVelocity, 2);
        pendulumGrapher.update(pendulumAngularVelocity * 180 / M_PI, 2);

        if (duration / 1000 >= 100) endFlag = 1;

        window->clear(sf::Color(50, 50, 50));
        window->draw(velGraphBorder);
        window->draw(pendulumbBorder);
        window->draw(angularGraphBorder);
        window->draw(upperRail);
        window->draw(lowerRail);
        window->draw(pendulum);
        window->draw(rightCircle);
        window->draw(leftCircle);
        window->draw(cartGrapher);
        window->draw(pendulumGrapher);
        window->display();
    }

    return 0;
}
