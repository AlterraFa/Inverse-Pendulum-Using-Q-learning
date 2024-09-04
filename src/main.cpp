#include <SFML/Graphics.hpp>
#include <cmath>
#include <tuple>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <termios.h>
#include <unistd.h>
#include "events.hpp"
#include "config.hpp"


const float cgravity = 9.81f;
const float timeStep = .005f;

using namespace std;

void force(float& position, float& velocity, float acceleration, float t) {
    velocity += acceleration * t;
    position += velocity * t;
}

void resetPosition(float& positionX, float& positionY, tuple<int, int> boundary){
    positionX = (positionX >= get<0>(boundary))? 0: positionX;
    positionY = (positionY >= get<1>(boundary))? 0: positionY;
}

class Logger {
private:
    std::ofstream logFile;

    // Function to get the current time as a string
    std::string getCurrentTime() {
        std::time_t now = std::time(nullptr);
        char buf[100];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        return buf;
    }

public:
    // Constructor: Opens the log file
    Logger(const std::string& filename) {
        logFile.open(filename, std::ios::trunc);
        if (!logFile.is_open()) {
            std::cerr << "Failed to open log file: " << filename << std::endl;
        }
    }

    // Destructor: Closes the log file
    ~Logger() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }

    // Method to write a log message with timestamp
    template <typename T>
    void log(const T& message) {
        if (logFile.is_open()) {
            logFile << "[" << getCurrentTime() << "] " << message << std::endl;
        } else {
            std::cerr << "Log file is not open!" << std::endl;
        }
    }
};

class Circle : public sf::Drawable{
    private:
        int radius;
        sf::CircleShape shape;

        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
            target.draw(shape, states);
        }

    public:

        Circle(int radius, int initX, int initY) : radius(radius), shape(radius){
            shape.setPointCount(200);
            shape.setFillColor(sf::Color(232, 109, 80));
            shape.setPosition(initX, initY);
        }

        void setPosition(float x, float y){
            shape.setPosition(x, y);
        }

        tuple<int, int> getPosition(){
            return {shape.getPosition().x, shape.getPosition().y};
        }

};

class Rectangle : public sf::Drawable {
private:
    sf::RectangleShape shape;  // The rectangle shape object

    // Override the draw method from sf::Drawable to define how this object is drawn
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        target.draw(shape, states);
    }

public:
    // Constructor to initialize the rectangle with size and position
    Rectangle(float width, float height, float initX, float initY) {
        shape.setSize(sf::Vector2f(width, height));
        shape.setFillColor(sf::Color::White);
        shape.setPosition(initX, initY);
    }

    // Method to set the position of the rectangle
    void setPosition(float x, float y) {
        shape.setPosition(x, y);
    }

    // Method to set the size of the rectangle
    void setSize(float width, float height) {
        shape.setSize(sf::Vector2f(width, height));
    }

    // Method to set the rotation of the rectangle
    void setRotation(float angle) {
        shape.setRotation(angle);
    }

    tuple<float, float> getPosition(){
        return {shape.getPosition().x, shape.getPosition().y};
    }

    // Method to get the positions of each corner of the rectangle
    std::vector<sf::Vector2f> getCornerPositions() {
        std::vector<sf::Vector2f> corners;

        // Get the size of the rectangle
        sf::Vector2f size = shape.getSize();
        
        // Get the current position of the rectangle
        sf::Vector2f position = shape.getPosition();

        // Define the corners relative to the position
        corners.push_back(shape.getTransform().transformPoint(sf::Vector2f(0, 0)));          // Top-left
        corners.push_back(shape.getTransform().transformPoint(sf::Vector2f(size.x, 0)));     // Top-right
        corners.push_back(shape.getTransform().transformPoint(sf::Vector2f(0, size.y)));     // Bottom-left
        corners.push_back(shape.getTransform().transformPoint(sf::Vector2f(size.x, size.y))); // Bottom-right

        return corners;
    }
};

int main() {
    // sf::ContextSettings settings;
    // settings.antialiasingLevel = 16;



    sf::RenderWindow window = conf::createWindow();


    // For esthetic only
    sf::RectangleShape lowerRail(sf::Vector2f(1550, 5));
    lowerRail.setFillColor(sf::Color::White);
    lowerRail.setPosition(150, 540);

    sf::RectangleShape upperRail(sf::Vector2f(1550, 5));
    upperRail.setFillColor(sf::Color::White);
    upperRail.setPosition(150, 520);

    const int pointCount = 50;   
    float outerRadius = 12.5f;   
    float innerRadius = 7.5f;   
    sf::Vector2f rightCenter(1700, 532.5); 
    sf::Vector2f leftCenter(150, 532.5); 

    sf::VertexArray rightCircle(sf::TriangleStrip, (pointCount + 1) * 2);
    sf::VertexArray leftCircle(sf::TriangleStrip, (pointCount + 1) * 2);

    for (int i = 0; i <= pointCount; ++i) {
        float angle = -M_PI / 2 + M_PI * i / pointCount;

        float xOuter = rightCenter.x + std::cos(angle) * outerRadius;
        float yOuter = rightCenter.y + std::sin(angle) * outerRadius;
        rightCircle[i * 2].position = sf::Vector2f(xOuter, yOuter);
        rightCircle[i * 2].color = sf::Color::White;

        float xInner = rightCenter.x + std::cos(angle) * innerRadius;
        float yInner = rightCenter.y + std::sin(angle) * innerRadius;
        rightCircle[i * 2 + 1].position = sf::Vector2f(xInner, yInner);
        rightCircle[i * 2 + 1].color = sf::Color::White;

        angle = M_PI / 2 + M_PI * i / pointCount;
        xOuter = leftCenter.x + std::cos(angle) * outerRadius;
        yOuter = leftCenter.y + std::sin(angle) * outerRadius;
        leftCircle[i * 2].position = sf::Vector2f(xOuter, yOuter);
        leftCircle[i * 2].color = sf::Color::White;

        xInner = leftCenter.x + std::cos(angle) * innerRadius;
        yInner = leftCenter.y + std::sin(angle) * innerRadius;
        leftCircle[i * 2 + 1].position = sf::Vector2f(xInner, yInner);
        leftCircle[i * 2 + 1].color = sf::Color::White;
    }
    // For esthetic only

    tuple<float, float> railBound = make_tuple(500, 1550);

    Rectangle cart(150, 50, 700, 505);
    float cartMass = 10;
    float inputForce = 100;
    float x, y;
    float velocity = 0;

    int inputType;
    while (window.isOpen()) {
        processEvents(window, inputType);        
        std::vector<sf::Vector2f> corners = cart.getCornerPositions();

        // tie(x, y) = cart.getPosition();            
        // float a = (inputType == RIGHT)? inputForce / cartMass: (inputType == LEFT)? -inputForce / cartMass: 0;
        // force(x, velocity, a, timeStep);
        // if (x + 700 / 2 <= get<0>(railBound)){
        //     velocity = 0;
        //     x = get<0>(railBound) - 700 / 2;
        // } else if (x + 700 / 2 >= get<1>(railBound)){
        //     velocity = 0;
        //     x = get<1>(railBound) - 700 / 2;
        // }
        // cart.setPosition(x, y);

        // cout << x << " " << get<1>(railBound) << "\n";


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
