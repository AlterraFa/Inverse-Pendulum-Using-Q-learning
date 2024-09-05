#include "obj.hpp"

// Rectangle class
Rectangle::Rectangle(float width, float height) {
    shape.setSize(sf::Vector2f(width, height));
    shape.setFillColor(sf::Color::White);
    shape.setOrigin(shape.getSize().x / 2, shape.getSize().y / 2);
}

void Rectangle::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(shape, states);
}

void Rectangle::setPosition(sf::Vector2f position) {
    shape.setPosition(position);
}

void Rectangle::setSize(float width, float height) {
    shape.setSize(sf::Vector2f(width, height));
}

void Rectangle::setRotation(float angle) {
    shape.setRotation(angle);
}

sf::Vector2f Rectangle::getPosition() {
    return sf::Vector2f(shape.getPosition().x, shape.getPosition().y);
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
Circle::Circle(float radius) : radius(radius), shape(radius) {
    shape.setPointCount(200);
    shape.setFillColor(sf::Color(232, 109, 80));
    shape.setOrigin(radius, radius);
}

void Circle::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(shape, states);
}

void Circle::setPosition(sf::Vector2f position) {
    shape.setPosition(position);
}

std::tuple<float, float> Circle::getPosition() {
    return std::make_tuple(shape.getPosition().x, shape.getPosition().y);
}



// Logging class
Logger::Logger(const std::string& filename){
    logFile.open(filename, std::ios::trunc);
    if (!logFile.is_open()) {
        std::cerr << "Failed to open log file: " << filename << std::endl;
    }
}