#include "obj.hpp"
#include "config.hpp"

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
    border.setPrimitiveType(sf::TrianglesStrip);
    background1.setPrimitiveType(sf::TriangleFan);
    background2.setPrimitiveType(sf::TriangleFan);
    background3.setPrimitiveType(sf::TriangleFan);
    background4.setPrimitiveType(sf::TriangleFan);
    renderTextureCreated = false;

    if (!renderTexture.create(size.x + thickness, size.y + thickness)) {
        std::cerr << "Failed to create render texture" << std::endl;
    }
    updateBackground();
    updateBorder();
    drawToTexture();
}

void Border::drawToTexture() {
    needsUpdate = false;
}

void Border::overrideColor(sf::Color color){
    for (size_t i = 0; i < border.getVertexCount(); i++){
        border[i].color = color;
    }
}

void Border::draw(sf::RenderTarget& target, sf::RenderStates state) const{
    if (!renderTextureCreated) {
        sf::ContextSettings settings = conf::getSettings();
        renderTexture.create(target.getSize().x, target.getSize().y, settings);
        renderTextureCreated = true;
        needsUpdate = true;
    }

    if (needsUpdate) {
        renderTexture.clear(sf::Color::Transparent);

        renderTexture.draw(background1, state);
        renderTexture.draw(background2, state);
        renderTexture.draw(background3, state);
        renderTexture.draw(background4, state);
        renderTexture.draw(border, state);
        renderTexture.display();
        needsUpdate = false;
    }

    sf::Sprite sprite(renderTexture.getTexture());
    target.draw(sprite, state);
}


void Border::append(sf::VertexArray& to, sf::VertexArray item){
    for (size_t i = 0; i < item.getVertexCount(); ++i) {
        to.append(item[i]);
    }
}

void Border::updateBackground(){
    append(background1, createQuarterCircle(180.0f, 270.0f, radius, sf::Vector2f(position.x + radius, position.y + radius), sf::Color(34, 34, 34)));
    append(background1, createQuarterCircle(.0f, -90.f, radius, sf::Vector2f(position.x + size.x - radius, position.y + radius), sf::Color(34, 34, 34)));
    append(background1, createQuarterCircle(90.0f, 0.0f, radius, sf::Vector2f(position.x + size.x - radius, position.y + size.y - radius), sf::Color(34, 34, 34)));
    append(background1, createQuarterCircle(180.f, 90.0f, radius, sf::Vector2f(position.x + radius, position.y + size.y - radius), sf::Color(34, 34, 34)));

    append(background2, createQuarterCircle(-90.f, 0.f, radius, sf::Vector2f(position.x + size.x - radius, position.y + radius), sf::Color(34, 34, 34)));
    append(background2, createQuarterCircle(90.0f, 0.0f, radius, sf::Vector2f(position.x + size.x - radius, position.y + size.y - radius), sf::Color(34, 34, 34)));

    append(background3, createQuarterCircle(0.0f, 90.0f, radius, sf::Vector2f(position.x + size.x - radius, position.y + size.y - radius), sf::Color(34, 34, 34)));
    append(background3, createQuarterCircle(180.f, 90.0f, radius, sf::Vector2f(position.x + radius, position.y + size.y - radius), sf::Color(34, 34, 34)));

    append(background4, createQuarterCircle(90.f, 180.0f, radius, sf::Vector2f(position.x + radius, position.y + size.y - radius), sf::Color(34, 34, 34)));
    append(background4, createQuarterCircle(180.0f, 270.0f, radius, sf::Vector2f(position.x + radius, position.y + radius), sf::Color(34, 34, 34)));
}

void Border::updateBorder(){
    append(border, createArc(180.0f, 90.0f, radius, sf::Vector2f(position.x + radius, position.y + radius), thickness, 20));
    append(border, createArc(270.0f, 90.0f, radius, sf::Vector2f(position.x + size.x - radius, position.y + radius), thickness, 20));
    append(border, createArc(0.0f, 90.0f, radius, sf::Vector2f(position.x + size.x - radius, position.y + size.y - radius), thickness, 20));
    append(border, createArc(90.0f, 90.0f, radius, sf::Vector2f(position.x + radius, position.y + size.y - radius), thickness, 20));
    append(border, createArc(180.0f, 90.0f, radius, sf::Vector2f(position.x + radius, position.y + radius), thickness, 20));
}

sf::VertexArray Border::createQuarterCircle(float startAngle, float endAngle, float radius, sf::Vector2f center, sf::Color color, int points) {
    sf::VertexArray arc(sf::TriangleFan, points + 2);

    arc[0].position = center;
    arc[0].color = color;

    float startRad = startAngle * M_PI / 180;
    float endRad = endAngle * M_PI / 180;
    float angleStep = (endRad - startRad) / points;

    for (int i = 0; i <= points; ++i) {
        float angle = startRad + i * angleStep; 
        float x = center.x + std::cos(angle) * radius;
        float y = center.y + std::sin(angle) * radius;

        arc[i + 1].position = sf::Vector2f(x, y);
        arc[i + 1].color = color;                
    }

    return arc;
}


sf::VertexArray Border::createArc(float startAngle, float angleLength, float radius, sf::Vector2f center, float thickness, int points) {
    sf::VertexArray arc(sf::TrianglesStrip, (points + 1) * 2);

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


//------------------------------------------ Graphing class --------------------------------------------------------//
Graphing::Graphing(size_t histSize, const sf::Font &f, std::vector<sf::Vector2f> graphBound, float scale, size_t maxVerticalLine, float barSpace, float textSize)
                   : graph(sf::LineStrip), font(f), graphBound(graphBound), graphScale(scale), horizontalBar(sf::Lines), verticalBar(sf::Lines),
                     maxLine(maxVerticalLine), lineCount(1), prevLineCount(0), moveLine(0.0f), textSize(textSize),
                     barSpace(barSpace), startTime(std::chrono::high_resolution_clock::now()){

    maxLineBorder = this->graphBound[1].x;
    midLine = (this->graphBound[0].y + this->graphBound[1].y) / 2;
    xHistory.assign(histSize, 0.0f);
    yHistory.assign(histSize, midLine);
    temp.assign(histSize, midLine);

    timer.push_back(0.0f);
    initHistory(histSize);
};
void Graphing::update(int newVelocity, int windowSize) {
    addVelocity(newVelocity, windowSize, graphScale);
    createHorizontalBars();
    createVerticalBars();

    graph.clear();
    for (size_t i = 0; i < yHistory.size(); i++) {
        graph.append(sf::Vertex(sf::Vector2f(xHistory[i], yHistory[i]), sf::Color(219, 192, 118)));
    }
}
void Graphing::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    target.draw(graph, states);
    target.draw(horizontalBar, states);
    target.draw(verticalBar, states);

    for (const auto &text : texts) {
        target.draw(text, states);
    }
    for (const auto &time : timerString) {
        target.draw(time, states);
    }
}
void Graphing::createVerticalBars() {
    timerString.clear();
    verticalBar.clear();

    if (lineCount <= maxLine - 1) {
        maxLineBorder -= 0.25f;
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
            oss << std::fixed << std::setprecision(1) << timer[iter];
            
            text.setString(oss.str());
            text.setCharacterSize(textSize);
            text.setFillColor(sf::Color::White);
            text.setPosition(position - 7, graphBound[1].y);
            timerString.push_back(text);

            verticalBar.append(sf::Vertex(sf::Vector2f(position, graphBound[0].y), sf::Color(180, 180, 180, 100)));
            verticalBar.append(sf::Vertex(sf::Vector2f(position, graphBound[1].y), sf::Color(180, 180, 180, 100)));
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
}
void Graphing::createHorizontalBars() {
    texts.clear();
    horizontalBar.clear();
    std::vector<float> horBarPos, magnitude;

    for (float i = midLine; i >= ((minVal <= graphBound[0].y)? midLine - round((midLine - minVal) / barSpace) * barSpace: graphBound[0].y); i -= barSpace){
        horBarPos.push_back(i);
        magnitude.push_back((midLine - i) / graphScale);
    }
    for (float i = midLine; i <= ((maxVal >= graphBound[1].y)? midLine + round((maxVal - midLine) / barSpace) * barSpace: graphBound[1].y); i += barSpace){
        horBarPos.push_back(i);
        magnitude.push_back((midLine - i) / graphScale);
    }
    
    if (minVal <= graphBound[0].y || maxVal >= graphBound[1].y){
        float maxBound = graphBound[1].y;
        float minBound = graphBound[0].y;
        std::transform(horBarPos.begin(), horBarPos.end(), horBarPos.begin(),
                        [this, minBound, maxBound](float position) {return minBound + ((position - minVal) * (maxBound - minBound)) / (maxVal - minVal);});
    }

    for (int i = 0; i < horBarPos.size(); i++){
        if (horBarPos[i] > graphBound[0].y && horBarPos[i] < graphBound[1].y){
            sf::Text text;
            text.setFont(font);
            text.setString(std::to_string(int(magnitude[i])));
            text.setCharacterSize(textSize);
            text.setFillColor(sf::Color::White);
            text.setPosition(graphBound[1].x + 5, horBarPos[i] - 7);
            texts.push_back(text);
            horizontalBar.append(sf::Vertex(sf::Vector2f(graphBound[0].x, horBarPos[i]), sf::Color(180, 180, 180, 100)));
            horizontalBar.append(sf::Vertex(sf::Vector2f(graphBound[1].x, horBarPos[i]), sf::Color(180, 180, 180, 100)));
            horizontalBar[0].position.x;
        }
    }    
}

void Graphing::addVelocity(int newVelocity, int windowSize, float scaleFactor) {
    temp.erase(temp.begin());
    float yPos = midLine - (newVelocity * scaleFactor);
    temp.push_back(yPos);
    yHistory = temp;
    smoothGraph(temp, windowSize);


    auto maxIt = std::max_element(yHistory.begin(), yHistory.end());
    maxVal = *maxIt;
    auto minIt = std::min_element(yHistory.begin(), yHistory.end());
    minVal = *minIt;

    if (minVal <= graphBound[0].y || maxVal >= graphBound[1].y) {
        float maxBound = graphBound[1].y;
        float minBound = graphBound[0].y;
        std::transform(yHistory.begin(), yHistory.end(), yHistory.begin(),
                    [this, minBound, maxBound](float position) {
                        return minBound + ((position - minVal) * (maxBound - minBound)) / (maxVal - minVal);
                    });
    }
}

void Graphing::smoothGraph(std::vector<float>& data, size_t windowSize) {
    if (data.size() < windowSize) return;

    std::vector<float> smoothedData;
    size_t halfWindow = windowSize / 2;

    // Apply the moving average filter
    for (size_t i = 0; i < data.size(); i++) {
        float sum = 0.0f;
        size_t count = 0;

        for (size_t j = i > halfWindow ? i - halfWindow : 0; 
            j <= i + halfWindow && j < data.size(); 
            j++) {
            sum += data[j];
            count++;
        }

        smoothedData.push_back(sum / count);
    }

    data = smoothedData;
}

void Graphing::initHistory(size_t histSize){
    std::iota(xHistory.begin(), xHistory.end(), 0);
    std::transform(xHistory.begin(), xHistory.end(), xHistory.begin(),
                    [=](size_t index){
                        return graphBound[0].x + (static_cast<float>(index) / static_cast<float>(histSize)) * (graphBound[1].x - graphBound[0].x);
                    });
}

//------------------------------------------ Logging class --------------------------------------------------------//
Logger::Logger(const std::string& filename){
    logFile.open(filename, std::ios::trunc);
    if (!logFile.is_open()) {
        std::cerr << "Failed to open log file: " << filename << std::endl;
    }
}