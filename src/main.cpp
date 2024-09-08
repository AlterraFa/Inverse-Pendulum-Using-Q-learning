#include "events.hpp"
#include "config.hpp"
#include "layout.hpp"
#include "obj.hpp"
#include "physics.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <iomanip>
#include <sstream>

void smoothGraph(std::vector<float>& data, size_t windowSize) {
    if (data.size() < windowSize) return;

    std::vector<float> smoothedData;
    size_t halfWindow = windowSize / 2;

    // Apply the moving average filter
    for (size_t i = 0; i < data.size(); i++) {
        float sum = 0.0f;
        size_t count = 0;

        // Calculate the average for the current window
        for (size_t j = i > halfWindow ? i - halfWindow : 0; 
             j <= i + halfWindow && j < data.size(); 
             j++) {
            sum += data[j];
            count++;
        }

        smoothedData.push_back(sum / count);
    }

    // Update the original data with the smoothed values
    data = smoothedData;
}


auto addVelocity(std::vector<float>& cartVelHistoryX, std::vector<float>& cartVelHistoryY,std::vector<float> &temp, int windowSize, int histSize, int newVelocity, float scaleFactor, std::vector<sf::Vector2f> graphBound) {
        float xPos = 150.0f + (float(cartVelHistoryY.size()) / histSize) * 500.0f;  // X position scaled to fit the graph width
        float yPos = 800 - (newVelocity * scaleFactor);  // Scale velocity to fit graph height (adjust scaling as needed)

        temp.erase(temp.begin());
        temp.push_back(yPos);
        cartVelHistoryY = temp;
        smoothGraph(temp, windowSize);


        auto maxIt = std::max_element(cartVelHistoryY.begin(), cartVelHistoryY.end());
        float maxVal = *maxIt;
        auto minIt = std::min_element(cartVelHistoryY.begin(), cartVelHistoryY.end());
        float minVal = *minIt;
        
        if(minVal <= graphBound[0].y || maxVal >= graphBound[1].y){
            float maxBound = graphBound[1].y;
            float minBound = graphBound[0].y;
            std::transform(cartVelHistoryY.begin(), cartVelHistoryY.end(), cartVelHistoryY.begin(),
                            [maxVal, minVal, minBound, maxBound](float position) {return minBound + ((position - minVal) * (maxBound - minBound)) / (maxVal - minVal);});
        }
        return std::make_tuple(minVal, maxVal);
    };


std::tuple<std::vector<sf::Text>, sf::VertexArray> horizontalBarCreate(float minVal, float maxVal, std::vector<sf::Vector2f> graphBound, float scaleFactor, const sf::Font& font, float fontSize, float barSpace){
    std::vector<float> horBarPos, magnitude;
    std::vector<sf::Text> texts;
    sf::VertexArray horBar(sf::Lines);

    float midLine = (graphBound[1].y + graphBound[0].y) / 2;
    for (float i = midLine; i >= ((minVal <= graphBound[0].y)? midLine - round((midLine - minVal) / barSpace) * barSpace: graphBound[0].y); i -= barSpace){
        horBarPos.push_back(i);
        magnitude.push_back((midLine - i) / scaleFactor);
    }
    for (float i = midLine; i <= ((maxVal >= graphBound[1].y)? midLine + round((maxVal - midLine) / barSpace) * barSpace: graphBound[1].y); i += barSpace){
        horBarPos.push_back(i);
        magnitude.push_back((midLine - i) / scaleFactor);
    }
    
    if (minVal <= graphBound[0].y || maxVal >= graphBound[1].y){
        float maxBound = graphBound[1].y;
        float minBound = graphBound[0].y;
        std::transform(horBarPos.begin(), horBarPos.end(), horBarPos.begin(),
                        [maxVal, minVal, minBound, maxBound](float position) {return minBound + ((position - minVal) * (maxBound - minBound)) / (maxVal - minVal);});
    }

    for (int i = 0; i < horBarPos.size(); i++){
        if (horBarPos[i] > graphBound[0].y && horBarPos[i] < graphBound[1].y){
            sf::Text text;
            text.setFont(font);
            text.setString(std::to_string(int(magnitude[i])));
            text.setCharacterSize(fontSize);
            text.setFillColor(sf::Color::White);
            text.setPosition(graphBound[1].x + 5, horBarPos[i] - 7);
            texts.push_back(text);
            horBar.append(sf::Vertex(sf::Vector2f(graphBound[0].x, horBarPos[i]), sf::Color(180, 180, 180, 100)));
            horBar.append(sf::Vertex(sf::Vector2f(graphBound[1].x, horBarPos[i]), sf::Color(180, 180, 180, 100)));
        }
    }    
    return std::make_tuple(texts, horBar);
}


auto verticalBarCreate(size_t& maxLine, float& maxLineBorder, int iteration, size_t& lineCount, size_t& prevLineCount, float& moveLine, std::vector<sf::Vector2f> graphBound, std::chrono::high_resolution_clock::time_point startTime, std::vector<float>& timer, const sf::Font& font){
    std::vector<sf::Text> timerString;
    sf::VertexArray vertBar(sf::Lines);
    if (iteration == 0 && lineCount <= maxLine - 1) {
        maxLineBorder -= 0.5f;
    }
    
    if (lineCount != prevLineCount){

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        timer.push_back(std::round(duration.count() / 100.f) / 10);

        prevLineCount = lineCount;
    }

    if (maxLineBorder <= lineCount * (graphBound[1].x - graphBound[0].x) / (lineCount + 1) + graphBound[0].x) {
        lineCount++;
        maxLineBorder = graphBound[1].x;
    }

    lineCount = std::min(lineCount, static_cast<size_t>(maxLine));

    if (lineCount == maxLine) {
        float maxMove = (maxLineBorder - graphBound[0].x) / lineCount;
        moveLine = (moveLine >= maxMove) ? 0 : moveLine + 0.125f;
    }

    
    float lineStep = (maxLineBorder - graphBound[0].x) / lineCount;
    int iter = 0;
    std::vector<float> positions;
    for (float i = graphBound[0].x; i <= maxLineBorder; i += lineStep) {
        float position = i - moveLine;
        if (position >= graphBound[0].x) {

            sf::Text text;
            text.setFont(font);
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(1) << 0;
            
            text.setString(oss.str());
            text.setCharacterSize(14);
            text.setFillColor(sf::Color::White);
            text.setPosition(position, graphBound[1].y);
            timerString.push_back(text);

            vertBar.append(sf::Vertex(sf::Vector2f(position, graphBound[0].y), sf::Color(180, 180, 180, 100)));
            vertBar.append(sf::Vertex(sf::Vector2f(position, graphBound[1].y), sf::Color(180, 180, 180, 100)));
        }
        iter++;
        positions.push_back(position);
    }
    if (lineCount == maxLine && positions[1] <= graphBound[0].x){
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        timer.erase(timer.begin());
        timer.push_back(std::round(duration.count() / 100.f) / 10);
    }
    return std::make_tuple(timerString, vertBar);
}


signed main() {


    // For esthetic only
    sf::RenderWindow window = conf::createWindow();
    sf::RectangleShape lowerRail, upperRail;
    std::tie(lowerRail, upperRail) = createRail(290, 20, 290, 1270);

    sf::VertexArray rightCircle, leftCircle;
    std::tie(leftCircle, rightCircle) = cap(12.5f, 7.5f, std::make_tuple(925, 292.5), 1270);

    Border pendulumbBorder(sf::Vector2f(100, 50), sf::Vector2f(1650, 500), 10.5f, sf::Color(232, 109, 80), 15.f);
    Border velGraphBorder(sf::Vector2f(100, 650), sf::Vector2f(600, 300), 5.f, sf::Color(219, 192, 118), 12.5f);
    // For esthetic only

    float inputForce = 350;

    Rectangle cart(150, 20);
    float cartMass = 50;
    float cartVel = 0;
    cart.setPosition(sf::Vector2f(900, 290));


    Circle pendulum(20);
    Circle pendulumRim(23);
    pendulumRim.overrideColor(sf::Color::White);
    float pendulumMass = 10.5;
    float angularVel = 0;
    float theta = M_PI;
    float length = 200;
    
    Circle pivot(20);
    Circle pivotRim(23);
    pivotRim.overrideColor(sf::Color::White);

    Rectangle rod(length, 6); 

    sf::Vector2f newCartPos, newPendulumPos, newRodPos;
    uint histSize = 2000, windowSize = 2;
    std::vector<float> cartVelHistoryX(histSize), cartVelHistoryY, temp(histSize, 800.f), timer;
    timer.push_back(0.f);
    std::iota(cartVelHistoryX.begin(), cartVelHistoryX.end(), 0);
    std::transform(cartVelHistoryX.begin(), cartVelHistoryX.end(), cartVelHistoryX.begin(), [=](size_t index){
        return 150.0f + (static_cast<float>(index) / static_cast<float>(histSize)) * 500.f;
    });
    
    std::vector<sf::Vector2f> graphBound = {sf::Vector2f(150, 700), sf::Vector2f(650, 900)};

    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/JetBrainsMonoNerdFont-Light.ttf")) {
        std::cerr << "Error: could not load font" << "\n";
        return -21321432;
    }

    auto startTime = std::chrono::high_resolution_clock::now(); 

    int inputType;
    float maxLineBorder = 650.f, moveLine = 0, minVal, maxVal, graphScale = 2.0f;
    bool addLine = false;
    size_t iteration = 0, maxLine = 10, lineCount = 1, prevLineCount = 0;
    while (window.isOpen()) {
        processEvents(window, inputType);        

        float force = (inputType == RIGHT && !(newCartPos.x >= std::get<1>(conf::railBound)))? inputForce: (inputType == LEFT && !(newCartPos.x <= std::get<0>(conf::railBound)))? -inputForce: 0;
        float friction = -((cartVel > 0) - (cartVel < 0)) * 5;
        float angularFriction = -((angularVel > 0) - (angularVel < 0)) * .1;

        Eigen::Vector4f stateVar = {cart.getPosition().x, cartVel, theta, angularVel};
        stateUpdate(stateVar, cartMass, pendulumMass, length, force + friction, angularFriction, conf::timeStep);

        newCartPos = sf::Vector2f(stateVar[0], cart.getPosition().y);
        cartVel = stateVar[1];
        theta = stateVar[2];
        angularVel = stateVar[3];
        newPendulumPos = sf::Vector2f(newCartPos.x - length * sinf(theta), newCartPos.y - length * cosf(theta));


        // Collision with rail bound
        if (newCartPos.x <= std::get<0>(conf::railBound)){
            cartVel = 0;
            newCartPos.x = std::get<0>(conf::railBound);
        }
        else if (newCartPos.x >= std::get<1>(conf::railBound)){
            cartVel = 0;
            newCartPos.x = std::get<1>(conf::railBound);
        }

        newRodPos.x = (newCartPos.x + newPendulumPos.x) / 2;
        newRodPos.y = (newCartPos.y + newPendulumPos.y) / 2;



        // Logging
        
        if (iteration == 0) {
            std::tie(minVal, maxVal) = addVelocity(cartVelHistoryX, cartVelHistoryY, temp, windowSize, histSize, cartVel, graphScale, graphBound);
        }


        sf::VertexArray graph(sf::LineStrip, cartVelHistoryY.size());
        for (int i = 0; i < cartVelHistoryY.size(); i++){
            graph[i].position = sf::Vector2f(cartVelHistoryX[i], cartVelHistoryY[i]);
            graph[i].color = sf::Color(219, 192, 118);
        }        



        sf::VertexArray verticalBar(sf::Lines), horizontalBar(sf::Lines);

        std::vector<float> horBarPos, magnitude;
        std::vector<sf::Text> texts, timerString;
        
        std::tie(texts, horizontalBar) = horizontalBarCreate(minVal, maxVal, graphBound, graphScale, font, 14.5f, 40.f);
        std::tie(timerString, verticalBar) = verticalBarCreate(maxLine, maxLineBorder, iteration, lineCount, prevLineCount, moveLine, graphBound, startTime, timer, font);        


        // Update position
        cart.setPosition(newCartPos);        
        pendulum.setPosition(newPendulumPos);
        pivot.setPosition(newCartPos);
        pendulumRim.setPosition(newPendulumPos);
        pivotRim.setPosition(newCartPos);
        rod.setPosition(newRodPos);
        rod.setRotation(-theta * 180 / M_PI + 90);

        window.clear(sf::Color(50, 50, 50));
        window.draw(velGraphBorder);
        window.draw(pendulumbBorder);
        window.draw(cart);
        window.draw(upperRail);
        window.draw(lowerRail);
        window.draw(rightCircle);
        window.draw(leftCircle);
        window.draw(rod);
        window.draw(pendulumRim);
        window.draw(pendulum);
        window.draw(pivotRim);
        window.draw(pivot);
        window.draw(horizontalBar);
        window.draw(verticalBar);
        for (auto time: timerString){
            window.draw(time);
        }
        for (auto text: texts){
            window.draw(text);
        }
        window.draw(graph);
        window.display();
        iteration = (++ iteration) % 2;
    }

    return 0;
}
