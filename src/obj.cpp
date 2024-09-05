#include "obj.hpp"

// Rectangle class
Rectangle::Rectangle(float width, float height, float initX, float initY) {
    shape.setSize(sf::Vector2f(width, height));
    shape.setFillColor(sf::Color::White);
    shape.setPosition(initX, initY);
}

void Rectangle::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(shape, states);
}

void Rectangle::setPosition(float x, float y) {
    shape.setPosition(x, y);
}

void Rectangle::setSize(float width, float height) {
    shape.setSize(sf::Vector2f(width, height));
}

void Rectangle::setRotation(float angle) {
    shape.setRotation(angle);
}

std::tuple<float, float> Rectangle::getPosition() {
    return {shape.getPosition().x, shape.getPosition().y};
}

std::vector<sf::Vector2f> Rectangle::getCornerPositions() {
    std::vector<sf::Vector2f> corners;
    sf::Vector2f size = shape.getSize();
    corners.push_back(shape.getTransform().transformPoint(sf::Vector2f(0, 0)));
    corners.push_back(shape.getTransform().transformPoint(sf::Vector2f(size.x, 0)));
    corners.push_back(shape.getTransform().transformPoint(sf::Vector2f(0, size.y)));
    corners.push_back(shape.getTransform().transformPoint(sf::Vector2f(size.x, size.y)));
    return corners;
}



// Circle class
Circle::Circle(int radius, int initX, int initY) : radius(radius), shape(radius) {
    shape.setPointCount(200);
    shape.setFillColor(sf::Color(232, 109, 80));
    shape.setPosition(initX, initY);
}

void Circle::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(shape, states);
}

void Circle::setPosition(float x, float y) {
    shape.setPosition(x, y);
}

std::tuple<int, int> Circle::getPosition() {
    return {static_cast<int>(shape.getPosition().x), static_cast<int>(shape.getPosition().y)};
}



// Logging class
Logger::Logger(const std::string& filename){
    logFile.open(filename, std::ios::trunc);
    if (!logFile.is_open()) {
        std::cerr << "Failed to open log file: " << filename << std::endl;
    }
}