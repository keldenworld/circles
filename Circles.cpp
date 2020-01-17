//https://github.com/keldenworld/circles
#include <iostream>
#include <chrono>
#include <ctime>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>
#include "circles.h"
#include "tools.h"

template <typename myContainer> 
int dynamics(myContainer* circles)
{
    sf::RenderWindow window(sf::VideoMode(1024, 768), "Hello Kelden World with SFML!");
    window.setVerticalSyncEnabled(false);

    sf::CircleShape shape(100);
    shape.setPointCount(MAXPoint);
    shape.setFillColor(sf::Color::Transparent);
    for (int i = 0; i < MAXOBJ; i++)
    {
        shape.setPosition(static_cast<float>(rand() % 1024 - 255) + 255, static_cast<float>(rand() % 728 - 255) + 255);
        shape.setRadius(static_cast<float>(rand() % (255 + 5) - 5));
        shape.setOutlineColor(sf::Color(rand() % 255, rand() % 255, rand() % 255));
        shape.setOutlineThickness(1);
        circles->add(shape);
    }

    sf::Clock clock;
    sf::Text text;
    text.setCharacterSize(32);
    text.setFillColor(sf::Color::White);
    sf::Font font;
    if (!font.loadFromFile("monofonto.ttf"))
        return -1;
    text.setFont(font);

    std::map<std::string, std::tuple<sf::Color, float>> timers;
    auto myfps = ftw::FPS();
    while (window.isOpen())
    {
        ftw::timethat timet(timers, "chrono Frametime (n-1) - RED", sf::Color::Red);
        {
            ftw::timethat timet(timers, "event loop + clear - GREEN", sf::Color::Green);
            sf::Event event;
            while (window.pollEvent(event))
                if (event.type == sf::Event::Closed)
                    window.close();
            window.clear();
        }
        {
            ftw::timethat timet(timers, "drawing circles - BLUE", sf::Color::Blue);
            for (auto& s : *circles)
                window.draw(s);
        }
        {
            ftw::timethat timet(timers, "log text and rectangles - GREEN", sf::Color::Green);
            myfps.update();
            text.setString("\nFPS (basics " + circles->type() + ") : " + std::to_string(myfps.get()) + "\n"
                + ftw::to_string(timers)
            );
            for (auto r : ftw::to_rectangle(timers))
                window.draw(r);
            window.draw(text);
        }
        {
            ftw::timethat timet(timers, "Display - MAGENTA", sf::Color::Magenta);
            window.display();
        }
    }
    return 0;
}

int main()
{
    {
        auto circles = std::make_unique<ftw::container_basedonvector<sf::CircleShape>>();
        dynamics<ftw::container_basedonvector<sf::CircleShape>>(circles.get()); 
    }
    {
        auto circles = std::make_unique<ftw::container_basedonarray<sf::CircleShape>>();
        dynamics<ftw::container_basedonarray<sf::CircleShape>>(circles.get());
    }
    {
        auto circles = std::make_unique<ftw::container_basedonlist<sf::CircleShape>>();
        dynamics<ftw::container_basedonlist<sf::CircleShape>>(circles.get());
    }
    return 0;
}
