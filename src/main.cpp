#include "events.hpp"
#include "config.hpp"
#include "layout.hpp"
#include "obj.hpp"
#include <sys/file.h>
#include <unistd.h>
#include <eigen3/Eigen/src/Core/IO.h>
#include <fstream>
#include <thread>
#include <string>
#include <mutex>
#include <fcntl.h>
#include <unistd.h>

void readInput(std::string &message, bool &updated, std::mutex &mutex) {
    std::string input;
    while (true) {
        std::getline(std::cin, input);
        if (input == "exit") {
            break;
        }
        std::lock_guard<std::mutex> lock(mutex); // Ensure safe update
        message = input;
        updated = true;
    }
}

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

    int inputType = 0;
    float cartForce = 350, pendulumForce = 30;

    float cartMass = 75;
    float pendulumMass = 10.5;
    float armLength = 200;
    float cartLinearVelocity, pendulumAngularVelocity, pendulumAttitude;
    sf::Vector2f cartPosition(900, 290);
    Pendulum pendulum(cartMass, pendulumMass, armLength, cartPosition);

    size_t histSize = 4000;
    std::vector<sf::Vector2f> cartGraphBound = {sf::Vector2f(150, 700), sf::Vector2f(650, 900)};
    std::vector<sf::Vector2f> pendulumGraphBound = {sf::Vector2f(800, 700), sf::Vector2f(1300, 900)};

    Graphing cartGrapher(histSize, font, cartGraphBound, 2.0f);
    Graphing pendulumGrapher(histSize, font, pendulumGraphBound, 2.0f);

    Eigen::VectorXd state(4);

    // Reading input from a medium
    std::ifstream file("pysrc/medium");

    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file" << std::endl;
        return -1;
    }

    float neuralInput = 0.0f;
    std::string test;

    // File descriptor for the lock
    int fd = open("pysrc/medium", O_RDONLY);
    if (fd == -1) {
        std::cerr << "Error: Cannot open file descriptor" << std::endl;
        return -1;
    }

    while (inputType != EXIT) {
        processEvents(window, inputType);

        // Lock the file for reading
        flock(fd, LOCK_SH);

        file.clear();
        file.seekg(0, std::ios::beg);

        while (std::getline(file, test)) {
            try {
                neuralInput = std::stof(test);
            } catch (const std::exception &e) {
                std::cerr << "Error: " << e.what() << std::endl;
                neuralInput = 0.0f; // Reset to a safe default value
            }
        }

        // Unlock the file
        flock(fd, LOCK_UN);

        std::cerr << neuralInput << std::endl;

        std::tie(cartPosition, cartLinearVelocity, pendulumAngularVelocity, pendulumAttitude) = pendulum.stateUpdate(
            cartForce, pendulumForce, conf::timeStep, neuralInput, conf::railBound, 12.5, 0.01);

        state << cartPosition.x, cartLinearVelocity, pendulumAngularVelocity, pendulumAttitude;
        std::cout << state.format(Eigen::IOFormat(Eigen::StreamPrecision, Eigen::DontAlignCols, ", ", ", ", "", "", "")) << std::endl;

        cartGrapher.update(cartLinearVelocity, 2);
        pendulumGrapher.update(pendulumAngularVelocity * 180 / M_PI, 2);

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

    // Close the file descriptor
    close(fd);

    return 0;
}
