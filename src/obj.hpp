#pragma once
#include <SFML/Graphics.hpp>
#include <tuple>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <iomanip>
#include <sstream>

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
        sf::VertexArray border, background1, background2, background3, background4;
        mutable sf::RenderTexture renderTexture;
        mutable bool renderTextureCreated;
        mutable bool needsUpdate;

        virtual void draw(sf::RenderTarget& target, sf::RenderStates state) const override;
        void drawToTexture();
        void updateBackground();
        void append(sf::VertexArray& to, sf::VertexArray item);
        void updateBorder();   
        sf::VertexArray createQuarterCircle(float startAngle, float endAngle, float radius, sf::Vector2f center, sf::Color color, int points = 20);
        sf::VertexArray createArc(float startAngle, float angleLength, float radius, sf::Vector2f center, float thickness, int points = 20);

    public:
        Border(sf::Vector2f position, sf::Vector2f size, float thickness, sf::Color color = sf::Color(232, 109, 80), float chamferRad = 10.0f);
        void overrideColor(sf::Color color);
};




class Graphing : public sf::Drawable {
    private:
        std::vector<float> xHistory, yHistory, temp, timer;
        std::vector<sf::Text> texts, timerString;
        sf::VertexArray graph, horizontalBar, verticalBar;
        sf::Font font;
        std::vector<sf::Vector2f> graphBound;
        float minVal, maxVal, graphScale, midLine;
        size_t maxLine, lineCount, prevLineCount;
        float maxLineBorder, moveLine, barSpace, textSize;
        std::chrono::high_resolution_clock::time_point startTime;

        void initHistory(size_t histSize);
        void smoothGraph(std::vector<float>& data, size_t windowSize);
        void addVelocity(int newVelocity, int windowSize, float scaleFactor);
        void createHorizontalBars();void createVerticalBars();
        void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
    public:
        Graphing(size_t histSize, const sf::Font& f, std::vector<sf::Vector2f> graphBound, float scale, size_t maxVerticalLine = 9, float barSpace = 40.f, float textSize = 12.0f);
        void update(int newVelocity, int windowSize);
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