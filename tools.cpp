#include "tools.h"

namespace ftw 
{
    void FPS::update() { counter++; frameTime += clock.restart(); }
    int FPS::get()
    {
        if (counter >= everyframe)
        {
            currentFT = frameTime.asSeconds() / counter;
            counter = 0;
            frameTime = sf::Time();
        }
        return static_cast<int>(1.0f / currentFT);
    }

    timethat::timethat(std::map<std::string, std::tuple<sf::Color, float>>& timers,
        std::string title, sf::Color col) :
        timers{ timers }, col{ col }, title{ title } {}
    timethat::~timethat()
    {
        auto end = std::chrono::system_clock::now();
        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds> (end - start).count();
        float nsf = static_cast<float>(ns);
        nsf += std::get<1>(timers[title]);
        timers[title] = std::make_tuple(col, nsf);
    }
    std::string timethat::to_string()
    {
        std::string msg = "";
        float total = 0.0f;
        float val = 0.0f;
        std::string percent, strval;
        for (auto e : timers)
            total += std::get<1>(e.second);
        for (auto e : timers)
        {
            val = std::get<1>(e.second);
            percent = std::to_string(static_cast<int>(round(val / total * 100))) + " %";
            strval =
                (val > 1'000'000 ? std::to_string(static_cast<int>(round(val / 1'000'000))) + " milli" :
                (val > 1'000 ? std::to_string(static_cast<int>(round(val / 1'000))) + " micro" :
                    std::to_string(static_cast<int>(round(val))) + "  nano")
                    );
            while (percent.size() < 5) percent = " " + percent;
            while (strval.size() < 3 + 6) strval = " " + strval;
            msg += percent + " - " + e.first + " : " + strval + "sec\n";
        }
        timers.clear();
        return msg;
    }
    std::vector<sf::RectangleShape> timethat::to_rectangle()
    {
        std::vector<sf::RectangleShape> rectangles;
        float lastpos = 0.0f;
        for (auto e : timers)
        {
            sf::Color c;
            float t;
            std::tie(c, t) = e.second;
            auto shape = sf::RectangleShape(sf::Vector2f(t / 100'000, 30));
            shape.setPosition(lastpos, 0);
            lastpos += t / 100'000;
            shape.setFillColor(c);
            rectangles.emplace_back(shape);
        }
        auto shape_60fps = sf::RectangleShape(sf::Vector2f(16'666'666 / 100'000, 20));
        shape_60fps.setFillColor(sf::Color::Red);
        shape_60fps.setPosition(0, 30);
        rectangles.emplace_back(shape_60fps);
        
        return rectangles;
    }


}