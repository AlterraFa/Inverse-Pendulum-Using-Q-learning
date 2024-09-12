#include "layout.hpp"
#include <cmath>

const int pointCount = 50;


std::tuple<sf::RectangleShape, sf::RectangleShape> createRail(float midline, float sepLength, float startPos, float length){

    sf::RectangleShape lowerRail(sf::Vector2f(length, 5));
    lowerRail.setFillColor(sf::Color::White);
    lowerRail.setPosition(startPos, midline + sepLength / 2);

    sf::RectangleShape upperRail(sf::Vector2f(length, 5));
    upperRail.setFillColor(sf::Color::White);
    upperRail.setPosition(startPos, midline - sepLength / 2);
    return std::make_tuple(lowerRail, upperRail);
}

std::tuple<sf::VertexArray, sf::VertexArray> cap(float outerRad, float innerRad, std::tuple<float, float> midPoint, float sepLength){

    sf::Vector2f rightCenter(std::get<0>(midPoint) + sepLength / 2, std::get<1>(midPoint)); 
    sf::Vector2f leftCenter(std::get<0>(midPoint) - sepLength / 2, std::get<1>(midPoint)); 

    sf::VertexArray rightCircle(sf::TriangleStrip, (pointCount + 1) * 2);
    sf::VertexArray leftCircle(sf::TriangleStrip, (pointCount + 1) * 2);

    for (int i = 0; i <= pointCount; ++i) {
        float angle = -M_PI / 2 + M_PI * i / pointCount;

        float xOuter = rightCenter.x + std::cos(angle) * outerRad;
        float yOuter = rightCenter.y + std::sin(angle) * outerRad;
        rightCircle[i * 2].position = sf::Vector2f(xOuter, yOuter);
        rightCircle[i * 2].color = sf::Color::White;

        float xInner = rightCenter.x + std::cos(angle) * innerRad;
        float yInner = rightCenter.y + std::sin(angle) * innerRad;
        rightCircle[i * 2 + 1].position = sf::Vector2f(xInner, yInner);
        rightCircle[i * 2 + 1].color = sf::Color::White;

        angle = M_PI / 2 + M_PI * i / pointCount;
        xOuter = leftCenter.x + std::cos(angle) * outerRad;
        yOuter = leftCenter.y + std::sin(angle) * outerRad;
        leftCircle[i * 2].position = sf::Vector2f(xOuter, yOuter);
        leftCircle[i * 2].color = sf::Color::White;

        xInner = leftCenter.x + std::cos(angle) * innerRad;
        yInner = leftCenter.y + std::sin(angle) * innerRad;
        leftCircle[i * 2 + 1].position = sf::Vector2f(xInner, yInner);
        leftCircle[i * 2 + 1].color = sf::Color::White;
    }
    return std::make_tuple(leftCircle, rightCircle); 
}
