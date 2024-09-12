#pragma once
#include <SFML/Graphics.hpp>
#include <tuple>


std::tuple<sf::RectangleShape, sf::RectangleShape> createRail(float midline, float sepLength, float startPos, float length);
std::tuple<sf::VertexArray, sf::VertexArray> cap(float outerRad, float innerRad, std::tuple<float, float> midPoint, float sepLength);
