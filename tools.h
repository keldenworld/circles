#pragma once

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
        void update() { counter++; frameTime += clock.restart(); }
        int get()
        {
            if (counter >= everyframe)
            {
                currentFT = frameTime.asSeconds() / counter;
                counter = 0;
                frameTime = sf::Time();
            }
            return static_cast<int>(1.0f / currentFT);
        }
    };

    std::vector<sf::RectangleShape> to_rectangle(std::map<std::string, std::tuple<sf::Color, float>>& timers)
    {
        std::vector<sf::RectangleShape> rectangles;
        float lastpos = 0.0f;
        for (auto e : timers)
        {
            sf::Color c;
            float t;
            std::tie(c, t) = e.second;
            auto shape = sf::RectangleShape(sf::Vector2f(t / 30'000, 20));
            if (sf::Color::Red == c)
                shape.setPosition(0, 20);
            else
            {
                shape.setPosition(lastpos, 0);
                lastpos += t / 30'000;
            }
            shape.setFillColor(c);
            rectangles.emplace_back(shape);
        }
        return rectangles;
    }

    std::string to_string(std::map<std::string, std::tuple<sf::Color, float>>& timers)
    {
        std::string msg = "";
        for (auto e : timers)
            msg += e.first + " : " + 
            (std::get<1>(e.second) > 1000 ? std::to_string(static_cast<int>(std::get<1>(e.second) / 1000)) + " micro" :
                std::to_string(static_cast<int>(std::get<1>(e.second))) + " nano") 
            + "sec\n";
        return msg;
    }

    class timethat
    {
        std::chrono::time_point<std::chrono::system_clock> start;
        std::map<std::string, std::tuple<sf::Color, float>>& timers;
        sf::Color col;
        std::string title;
    public:
        timethat(std::map<std::string, std::tuple<sf::Color, float>>& timers, std::string title, sf::Color col) :
            start{ std::chrono::system_clock::now() },
            timers{ timers }, col{ col },
            title{ title }{}
        ~timethat()
        {
            auto end = std::chrono::system_clock::now();
            auto ns = std::chrono::duration_cast<std::chrono::nanoseconds> (end - start).count();
            timers[title] = std::make_tuple(col, static_cast<float>(ns));
        }
    };
}
