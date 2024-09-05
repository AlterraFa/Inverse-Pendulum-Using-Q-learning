#pragma once
#include <SFML/Graphics.hpp>
#include <tuple>
#include <stdio.h>
#include <iostream>
#include <fstream>

#ifndef Shapes_HPP
#define Shapes_HPP

class Rectangle : public sf::Drawable {
    private:
        sf::RectangleShape shape;  // The rectangle shape object

        // Override the draw method from sf::Drawable to define how this object is drawn
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    public:
        Rectangle(float width, float height);
        void setPosition(sf::Vector2f position);
        void setSize(float width, float height);
        void setRotation(float angle);
        sf::Vector2f getPosition();
        std::vector<sf::Vector2f> getCornerPositions();
};


class Circle : public sf::Drawable{
    private:
        int radius;
        sf::CircleShape shape;

        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    public:
        Circle(float radius);
        void setPosition(sf::Vector2f position);
        std::tuple<float, float> getPosition();  
};

class Logger {
    private:
        std::ofstream logFile;
        std::string getCurrentTime();
    
    public:
        Logger(const std::string& filename);
        ~Logger();
        template <typename T>
        void log(const T& message);
};


#endif