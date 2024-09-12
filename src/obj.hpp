#pragma once
#include <math.h>
#include <SFML/Graphics.hpp>
#include <tuple>
#include <stdio.h>
#include <iostream>
#include <fstream>
#define _USE_MATH_DEFINES
#undef __STRICT_ANSI__
#include <cmath>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <eigen3/Eigen/Dense>
#include "config.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef Shapes_HPP
#define Shapes_HPP

class Rectangle : public sf::Drawable {
    private:
        sf::RectangleShape shape;
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    public:
        Rectangle();
        Rectangle(float width, float height);
        void overrideColor(sf::Color color);
        void setPosition(sf::Vector2f position);
        void setSize(float width, float height);
        void setRotation(float angle);
        sf::Vector2f getPosition();
        std::vector<sf::Vector2f> getCornerPositions();
};


class Circle : public sf::Drawable{
    private:
        sf::CircleShape shape;

        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    public:
        Circle();
        Circle(float radius);
        void setPosition(sf::Vector2f position);
        void overrideColor(sf::Color color);
        void setRadius(float radius);
        std::tuple<float, float> getPosition();  
};



class Pendulum : public sf::Drawable{
    private:
        float cartMass, pendulumMass;
        float cartLinearVelocity, pendulumAngularVelocity;
        float pendulumAttitude;
        sf::Vector2f cartPosition, pendulumPosition, rodPosition;
        float armLength;

        Rectangle cart, rod;
        Circle pendulum, pendulumRim, pivot, pivotRim;

        Eigen::Vector4f funct(Eigen::Vector4f x, float force, float forceS);
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    public:
        Pendulum(float cartMass, float pendulumMass, float armLength, sf::Vector2f cartPosition, float pendulumAttitude = M_PI);
        void dimension(sf::Vector2f cartDimension = sf::Vector2f(150, 20), float pendulumRadius = 20, float pivotRadius = 20, float rodThickness = 6);
        std::tuple<sf::Vector2f, float, float> stateUpdate(float cartForce, float pendulumForce, float timeStep, int inputType, 
                                                           sf::Vector2f railBound = static_cast<sf::Vector2f>(conf::createWindow(conf::getSettings()) -> getSize()),
                                                           float slidingFriction = 5, float angularFriction = .005);
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
        void overrideBorderColor(sf::Color color);
        void overrideBackgroundColor(sf::Color color);
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
