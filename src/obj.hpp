#pragma once
#include <SFML/Graphics.hpp>
#include <tuple>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cmath>

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
        void overrideColor(sf::Color color);
        std::tuple<float, float> getPosition();  
};

class Border : public sf::Drawable{
    private:
        sf::Vector2f position;
        sf::Vector2f size;
        float thickness;
        float radius;
        sf::Color color;
        sf::VertexArray lines;
        sf::VertexArray corners;

        virtual void draw(sf::RenderTarget& target, sf::RenderStates state) const override;
        void updateLines();
        void setQuad(int index, sf::Vector2f start, sf::Vector2f end, float thickness);
        void updateCorners();   
        void appendArc(const sf::VertexArray& arc);
        sf::VertexArray createArc(float startAngle, float angleLength, float radius, sf::Vector2f center, float thickness);

    public:
        Border(sf::Vector2f position, sf::Vector2f size, float thickness, sf::Color color = sf::Color(232, 109, 80), float chamferRad = 10.0f);
        void overrideColor(sf::Color color);
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