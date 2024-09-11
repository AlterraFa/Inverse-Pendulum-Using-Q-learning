#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include "events.hpp"
#include "config.hpp"
#include "layout.hpp"
#include "obj.hpp"

#define STRINGIFY(x) #x
#define MACRO_STRIGIFY(x) STRINGIFY(x)

namespace py = pybind11;

PYBIND11_MODULE(PendulumSimulation, m){
    m.doc() = "Python binding for Pendulum simulation";

    m.attr("StyleNone") = 0;
    m.attr("StyleTitlebar") = 1;
    m.attr("StyleResize") = 2;
    m.attr("StyleClose") = 4;
    m.attr("StyleFullscreen") = 8;
    m.attr("StyleDefault") = 7;


    py::class_<sf::Drawable>(m, "Drawable");

    py::class_<sf::Vector2f>(m, "Vector2f")
        .def(py::init<float, float>(), py::arg("x") = 0.0f, py::arg("y") = 0.0f)
        .def_readwrite("x", &sf::Vector2f::x)  
        .def_readwrite("y", &sf::Vector2f::y) 
        .def("__repr__", [](const sf::Vector2f &v) { 
            return "<Vector2f x=" + std::to_string(v.x) + ", y=" + std::to_string(v.y) + ">";
        })
        .def(py::self + py::self)   
        .def(py::self - py::self)
        .def(py::self * float())
        .def(py::self / float());

    py::class_<sf::RectangleShape, sf::Drawable>(m, "RectangleShape")
        .def(py::init<sf::Vector2f>(), py::arg("size") = sf::Vector2f(0, 0))
        .def("set_size", &sf::RectangleShape::setSize)
        .def("set_fill_color", &sf::RectangleShape::setFillColor);

    py::class_<sf::CircleShape, sf::Drawable>(m, "CircleShape")
        .def(py::init<float>(), py::arg("radius") = 0)
        .def("set_radius", &sf::CircleShape::setRadius)
        .def("set_fill_color", &sf::CircleShape::setFillColor);

    py::class_<sf::Color>(m, "Color")
        .def(py::init<>())  // Default constructor
        .def(py::init<sf::Uint8, sf::Uint8, sf::Uint8, sf::Uint8>(), 
             py::arg("r") = 0, py::arg("g") = 0, py::arg("b") = 0, py::arg("a") = 255) // Constructor with RGBA values
        .def_readwrite("r", &sf::Color::r)  // Red component
        .def_readwrite("g", &sf::Color::g)  // Green component
        .def_readwrite("b", &sf::Color::b)  // Blue component
        .def_readwrite("a", &sf::Color::a); // Alpha component

    // Binding commonly used color constants
    m.attr("Color_Black") = sf::Color::Black;
    m.attr("Color_White") = sf::Color::White;
    m.attr("Color_Red") = sf::Color::Red;
    m.attr("Color_Green") = sf::Color::Green;
    m.attr("Color_Blue") = sf::Color::Blue;
    m.attr("Color_Yellow") = sf::Color::Yellow;
    m.attr("Color_Magenta") = sf::Color::Magenta;
    m.attr("Color_Cyan") = sf::Color::Cyan;
    m.attr("Color_Transparent") = sf::Color::Transparent;

    // Registering sf::ContextSettings
    py::class_<sf::ContextSettings>(m, "ContextSettings")
        .def(py::init<>())
        .def_readwrite("depthBits", &sf::ContextSettings::depthBits)
        .def_readwrite("antialiasingLevel", &sf::ContextSettings::antialiasingLevel)
        .def_readwrite("majorVersion", &sf::ContextSettings::majorVersion)
        .def_readwrite("minorVersion", &sf::ContextSettings::minorVersion)
        .def_readwrite("attributeFlags", &sf::ContextSettings::attributeFlags)
        .def_readwrite("sRgbCapable", &sf::ContextSettings::sRgbCapable);


    
    // Register sf::VideoMode
    py::class_<sf::VideoMode>(m, "VideoMode")
        .def(py::init<unsigned int, unsigned int, unsigned int>(), py::arg("width") = 800, py::arg("height") = 600, py::arg("bitsPerPixel") = 32)
        .def_readwrite("width", &sf::VideoMode::width)
        .def_readwrite("height", &sf::VideoMode::height)
        .def_readwrite("bitsPerPixel", &sf::VideoMode::bitsPerPixel)
        .def_static("getDesktopMode", &sf::VideoMode::getDesktopMode)
        .def_static("getFullscreenModes", &sf::VideoMode::getFullscreenModes);
        

    // Register sf::Window    
    py::class_<sf::Window>(m, "Window")
        .def(py::init<>())  // Default constructor
        .def(py::init<sf::VideoMode, const sf::String&, sf::Uint32, const sf::ContextSettings&>(),
             py::arg("mode"), py::arg("title"), py::arg("style") = 7, py::arg("settings") = sf::ContextSettings())
        .def("create",
             static_cast<void (sf::Window::*)(sf::VideoMode, const sf::String&, sf::Uint32, const sf::ContextSettings&)>(&sf::Window::create),
             py::arg("mode"), py::arg("title"), py::arg("style") = 7, py::arg("settings") = sf::ContextSettings(),
             "Creates the window with the specified settings.")
        .def("isOpen", &sf::Window::isOpen)
        .def("close", &sf::Window::close)
        .def("pollEvent", &sf::Window::pollEvent)
        .def("display", &sf::Window::display)
        .def("setTitle", &sf::Window::setTitle, py::arg("title"))
        .def("setFramerateLimit", &sf::Window::setFramerateLimit, py::arg("limit"));

    // Register sf::RenderWindow
    py::class_<sf::RenderWindow, sf::Window>(m, "RenderWindow")
        .def(py::init<sf::VideoMode, const std::string&, sf::Uint32, const sf::ContextSettings&>(),
             py::arg("mode"), py::arg("title"), py::arg("style") = 7, py::arg("settings") = sf::ContextSettings())
        .def("createWindow", &conf::createWindow, py::return_value_policy::reference)
        .def("isOpen", &sf::RenderWindow::isOpen)
        .def("close", &sf::RenderWindow::close)
        .def("clear", &sf::RenderWindow::clear, py::arg("color") = sf::Color::Black)
        .def("display", &sf::RenderWindow::display)
        .def("draw", [](sf::RenderWindow &self, const sf::Drawable &drawable) { self.draw(drawable); }, py::arg("drawable"))
        .def("setView", &sf::RenderWindow::setView)
        .def("getView", &sf::RenderWindow::getView)
        .def("setFramerateLimit", &sf::RenderWindow::setFramerateLimit)
        .def("setVerticalSyncEnabled", &sf::RenderWindow::setVerticalSyncEnabled);


    // Register other SFML drawable types like sf::Sprite
    py::class_<sf::Sprite, sf::Drawable>(m, "Sprite")
        .def(py::init<>())
        .def("setPosition", py::overload_cast<float, float>(&sf::Sprite::setPosition))
        .def("setTexture", &sf::Sprite::setTexture, py::arg("texture"), py::arg("resetRect") = false);


    // in config.hpp
    m.attr("UP") = UP;
    m.attr("DOWN") = DOWN;
    m.attr("LEFT") = LEFT;
    m.attr("RIGHT") = RIGHT;
    m.attr("gravity") = conf::cgravity;
    m.def("createWindow", &conf::createWindow, "Create the SFML window with predefined settings");
    m.def("getSettings", &conf::getSettings, "Get the default settings, can be changed");

    // in events.cpp
    m.def("processEvents", &processEvents, "Process predefined events");


    // in layout.cpp
    m.def("createRail", &createRail, "Create railings for cart");
    m.def("cap", &cap, "Cap of the rail for aesthetic");
    
    // in obj.cpp
    py::class_<Rectangle>(m, "Rectangle")
        .def(py::init<float, float>())
        .def("setPosition", &Rectangle::setPosition, R"pbdoc(Set the position of the rectangle (at the center))pbdoc")
        .def("getCornerPositions", &Rectangle::getCornerPositions, R"pbdoc(Returns 4 corners position)pbdoc")
        .def("getPosition", &Rectangle::getPosition, R"pbdoc(Get the center position of Rectangle)pbdoc")
        .def("setRotation", &Rectangle::setRotation, R"pbdoc(Rotate the rectangle (radian))pbdoc")
        .def("setSize", &Rectangle::setSize, R"pbdoc(
            Set the size of the rectangle  
        )pbdoc");
    
    py::class_<Circle>(m, "Circle")
        .def(py::init<float>())
        .def("getPosition", &Circle::getPosition, R"pbdoc(Get the current center position of circe)pbdoc")
        .def("overrideColor", &Circle::overrideColor, R"pbdoc(Override current default Circle color (232, 109, 80))pbdoc")
        .def("setPosition", &Circle::setPosition, R"pbdoc(Set the circle center position)pbdoc");
    
    py::class_<Border>(m, "Border")
        .def(py::init<sf::Vector2f, sf::Vector2f, float, sf::Color, float>())
        .def("overrideBackgroundColor", &Border::overrideBackgroundColor, R"pbdoc(Override the default background color (34, 34, 34))pbdoc")
        .def("overrideBorderColor", &Border::overrideBorderColor, R"pbdoc(Override the current border color (232, 109, 80))pbdoc");
    
    py::class_<Graphing>(m, "Graphing")
        .def(py::init<size_t, sf::Font&, std::vector<sf::Vector2f>, float, size_t, float, float>(), R"pbdoc(
            A dynamic animated graph that can change size if the line of graph exceeds the bound
        )pbdoc")
        .def("update", &Graphing::update, R"pbdoc(Updates the graph, should be used in a loop)pbdoc");
    

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRIGIFY(VERSION_INFO);
#else
    m.attr("__version") = "dev";
#endif
}
