#pragma once
#include <SFML/Graphics.hpp>
#include <tuple>
#include <iostream>

#define UP      1
#define DOWN    2
#define LEFT    3
#define RIGHT   4

namespace conf {
    const float cgravity = 9.81f;
    const float timeStep = .015f;
    const std::tuple<float, float> railBound = std::make_tuple(365, 1485);
    const std::tuple<int, int> boundary = std::make_tuple(1920, 1080);
    
    inline sf::ContextSettings getSettings() {
        sf::ContextSettings settings;
        settings.antialiasingLevel = 10;
        settings.majorVersion = 4;
        settings.minorVersion = 6;
        return settings;
    }

    inline const sf::ContextSettings& settings = getSettings();
    inline sf::RenderWindow createWindow() {
        // Use the settings and boundary from the namespace
        return sf::RenderWindow(
            sf::VideoMode(std::get<0>(boundary), std::get<1>(boundary)),
            "Pendulum simulation",
            sf::Style::Default,
            settings
        );
    }
}