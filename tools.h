#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>
#include <chrono>

namespace ftw
{
    class FPS
    {
        sf::Clock clock;
        int counter{ 1 };
        sf::Time frameTime{ clock.restart() };
        int everyframe;
        float currentFT{ 0.0f };
    public:
        FPS(int everyframe = 10) : everyframe(everyframe) {}
        void update();
        int get();
    };
    
    class timethat
    {
        std::chrono::time_point<std::chrono::system_clock>
            start{ std::chrono::system_clock::now() };
        std::map<std::string, std::tuple<sf::Color, float>>& timers;
        sf::Color col;
        std::string title;
    public:
        timethat(std::map<std::string, std::tuple<sf::Color, float>>& timers,
            std::string title, sf::Color col);
        ~timethat();
        std::string to_string();
        std::vector<sf::RectangleShape> to_rectangle();
    };
}
