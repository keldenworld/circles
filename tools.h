#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>
#include <chrono>

const unsigned int MAXOBJ = 1024;
const unsigned int MAXPoint = 16;
const int MAXTerrain = 1024 * 2;
const int MINScreen_X = 0;
const int MINScreen_Y = 0;
const int MAXScreen_X = 1024;
const int MAXScreen_Y = 768;

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
    
    class TestColors
    {
        size_t TestColorsId = 0;
        std::vector<std::tuple<std::string, sf::Color>> VTestColors
        {
            std::make_tuple("Orange", sf::Color(0xFF, 0xA5, 0x00)),
            std::make_tuple("DeepSkyblue",sf::Color(0x00, 0xBF, 0xFF)),
            std::make_tuple("DarkViolet", sf::Color(0x94, 0x00, 0xD3)),
            std::make_tuple("LightPink", sf::Color(0xFF, 0xB6, 0xC1)),
            std::make_tuple("Purple", sf::Color(0x80, 0x00, 0x80)),
            std::make_tuple("DimGray",sf::Color(0x69, 0x69, 0x69)),
            std::make_tuple("Magenta", sf::Color::Magenta),
            std::make_tuple("Cyan", sf::Color::Cyan),
            std::make_tuple("Green",sf::Color::Green),
            std::make_tuple("Yellow",sf::Color::Yellow),
            std::make_tuple("White",sf::Color::White),
            std::make_tuple("Black",sf::Color::Black)
        };
        TestColors() {}
    public:
        static TestColors& instance()
        {
            static TestColors theSingleInstance;
            return theSingleInstance;
        }
        std::tuple<std::string, sf::Color>& next()
        {
            if (TestColorsId >= VTestColors.size())
                TestColorsId = 0;
            return VTestColors[TestColorsId++];
        };
        void reset(){TestColorsId = 0;};
    };

    class timethat
    {
        std::chrono::time_point<std::chrono::system_clock>
            start{ std::chrono::system_clock::now() };
        std::map<std::string, std::tuple<std::string, sf::Color, float>>& timers;
        std::tuple<std::string, sf::Color> col;
        std::string title;
    public:
        timethat(std::map<std::string, std::tuple<std::string, sf::Color, float>>& timers,
            std::string title, std::tuple<std::string, sf::Color> col = TestColors::instance().next());
        ~timethat();
        std::string to_string();
        std::vector<sf::RectangleShape> to_rectangle();
    };
}
