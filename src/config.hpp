#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <tuple>
#include <iostream>

#define EXIT    -1
#define UP      1
#define DOWN    2
#define LEFT    3
#define RIGHT   4

namespace conf {
    const float cgravity = 9.81f;
    const float timeStep = .015f;
    const sf::Vector2f railBound = sf::Vector2f(365, 1485);
    const std::tuple<int, int> boundary = std::make_tuple(1920, 1080);
    
    inline sf::ContextSettings getSettings(int antialiasLevel = 8) {
        sf::ContextSettings settings;
        settings.antialiasingLevel = antialiasLevel;
        settings.majorVersion = 4;
        settings.minorVersion = 6;
        return settings;
    }

    inline const sf::ContextSettings& settings = getSettings();


    inline  std::unique_ptr<sf::RenderWindow> createWindow(sf::ContextSettings settings, std::tuple<float, float> boundary = std::make_tuple(1920, 1080)) {
        return std::make_unique<sf::RenderWindow>(
            sf::VideoMode(std::get<0>(boundary), std::get<1>(boundary)),
            "Pendulum simulation",
            sf::Style::Default,
            settings
        );
    }
}
