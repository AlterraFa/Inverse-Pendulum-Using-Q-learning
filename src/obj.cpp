#include "obj.hpp"

//------------------------------------------ Rectangle class --------------------------------------------------------//
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




//------------------------------------------ Circle class --------------------------------------------------------//
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

void Circle::overrideColor(sf::Color color){
    shape.setFillColor(color);
}

std::tuple<float, float> Circle::getPosition() {
    return std::make_tuple(shape.getPosition().x, shape.getPosition().y);
}




//------------------------------------------ Border class --------------------------------------------------------//
Border::Border(sf::Vector2f position, sf::Vector2f size, float thickness, sf::Color color, float chamferRad)
    : position(position), size(size), thickness(thickness), color(color), radius(chamferRad), needsUpdate(true) {
    lines.setPrimitiveType(sf::Quads);
    lines.resize(16);
    renderTextureCreated = false;
    updateLines();
}

void Border::overrideColor(sf::Color color){
    for (size_t i = 0; i < lines.getVertexCount(); i++){
        lines[i].color = color;
    }

    for (size_t i = 0; i < corners.getVertexCount(); i++){
        corners[i].color = color;
    }
}

void Border::draw(sf::RenderTarget& target, sf::RenderStates state) const{
    if (!renderTextureCreated) {
        // Create the render texture once
        sf::ContextSettings settings;
        settings.antialiasingLevel = 16; // Reduced anti-aliasing level
        renderTexture.create(target.getSize().x, target.getSize().y, settings);
        renderTextureCreated = true;
        needsUpdate = true;
    }

    if (needsUpdate) {
        renderTexture.clear(sf::Color::Transparent);

        // Create a rectangle shape to fill the inner region with the specific color
        sf::RectangleShape innerRect(size);
        innerRect.setPosition(position);

        // Replace the following line with the exact color value extracted from your image
        innerRect.setFillColor(sf::Color(34, 34, 34)); // Example color, replace with your actual color value

        renderTexture.draw(innerRect); // Draw the inner filled rectangle

        renderTexture.draw(lines, state);
        renderTexture.draw(corners, state);
        renderTexture.display();
        needsUpdate = false;
    }

    // Draw the texture as a sprite onto the main target
    sf::Sprite sprite(renderTexture.getTexture());
    target.draw(sprite, state);
}

void Border::updateLines() {
    setQuad(0, sf::Vector2f(position.x + radius, position.y), sf::Vector2f(position.x + size.x - radius, position.y), thickness);
    setQuad(4, sf::Vector2f(position.x + size.x, position.y + radius), sf::Vector2f(position.x + size.x, position.y + size.y - radius), thickness);
    setQuad(8, sf::Vector2f(position.x + size.x - radius, position.y + size.y), sf::Vector2f(position.x + radius, position.y + size.y), thickness);
    setQuad(12, sf::Vector2f(position.x, position.y + size.y - radius), sf::Vector2f(position.x, position.y + radius), thickness);
    updateCorners();
}

void Border::setQuad(int index, sf::Vector2f start, sf::Vector2f end, float thickness) {
    sf::Vector2f direction = end - start;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    direction /= length;
    sf::Vector2f offset(-direction.y * thickness / 2, direction.x * thickness / 2);

    lines[index].position = start + offset;
    lines[index + 1].position = start - offset;
    lines[index + 2].position = end - offset;
    lines[index + 3].position = end + offset;

    for (int i = 0; i < 4; ++i) {
        lines[index + i].color = color;
    }
}

void Border::updateCorners() {
    corners.clear();
    corners.setPrimitiveType(sf::TrianglesStrip);
    appendArc(createArc(180.0f, 90.0f, radius, sf::Vector2f(position.x + radius, position.y + radius), thickness));
    appendArc(createArc(270.0f, 90.0f, radius, sf::Vector2f(position.x + size.x - radius, position.y + radius), thickness));
    appendArc(createArc(0.0f, 90.0f, radius, sf::Vector2f(position.x + size.x - radius, position.y + size.y - radius), thickness));
    appendArc(createArc(90.0f, 90.0f, radius, sf::Vector2f(position.x + radius, position.y + size.y - radius), thickness));
}

void Border::appendArc(const sf::VertexArray& arc) {
    for (size_t i = 0; i < arc.getVertexCount(); ++i) {
        corners.append(arc[i]);
    }
}

sf::VertexArray Border::createArc(float startAngle, float angleLength, float radius, sf::Vector2f center, float thickness) {
    int points = 50;
    sf::VertexArray arc(sf::TriangleStrip, (points + 1) * 2);

    float startRad = startAngle * M_PI / 180;
    float endRad = (startAngle + angleLength) * M_PI / 180;
    float angleStep = (endRad - startRad) / points;

    for (int i = 0; i <= points; ++i) {
        float angle = startRad + i * angleStep;
        float xOuter = center.x + std::cos(angle) * (radius + thickness / 2);
        float yOuter = center.y + std::sin(angle) * (radius + thickness / 2);
        arc[i * 2].position = sf::Vector2f(xOuter, yOuter);
        arc[i * 2].color = color;

        float xInner = center.x + std::cos(angle) * (radius - thickness / 2);
        float yInner = center.y + std::sin(angle) * (radius - thickness / 2);
        arc[i * 2 + 1].position = sf::Vector2f(xInner, yInner);
        arc[i * 2 + 1].color = color;
    }
    return arc;
}






//------------------------------------------ Logging class --------------------------------------------------------//
Logger::Logger(const std::string& filename){
    logFile.open(filename, std::ios::trunc);
    if (!logFile.is_open()) {
        std::cerr << "Failed to open log file: " << filename << std::endl;
    }
}