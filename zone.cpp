#include "tools.h"
#include "zone.h"

namespace ftw
{
    zone::zone(int& idEngine, std::vector<physicsData>& vDatas,
        std::vector<std::vector<size_t>>& vCollisions,
        sf::Vector2f min, sf::Vector2f max)
        : min{ min }, max{ max }, idEngine{ idEngine }, vCollisions{vCollisions}
    {
        emplace_back(vDatas);
        if (childs.size() == 0)
            for (auto i = 0; i < data.size(); i++)
                for (auto& e : data)
                {
                    while (vCollisions.size() < data[i].currentId + 1)
                        vCollisions.emplace_back(std::vector<size_t>());
                    vCollisions[data[i].currentId].emplace_back(e.currentId);
                }
    }

    void zone::to_string(std::vector<std::tuple<std::string, float, float>>& strzlog)
    {
        if (childs.size() == 0)
            strzlog.emplace_back(
                std::tuple(
                    std::to_string(id) + " : " + std::to_string(this->data.size()),
                    this->min.x, this->min.y));
        else
            for (auto& z : childs)
                z->to_string(strzlog);
    }

    void zone::to_circle(std::vector<sf::CircleShape>& circlesPerZone)
    {
        if (childs.size() == 0)
            for (auto& e : data)
            {
                auto c = sf::CircleShape(1);
                auto x = e.position.x + e.radius;
                auto y = e.position.y + e.radius;
                c.setPosition(x, y);
                if (id == 3) //debug
                    circlesPerZone.emplace_back(c);
            }
        else
            for (auto& z : childs)
                z->to_circle(circlesPerZone);
    }

    void zone::to_rectangle(std::vector<sf::RectangleShape>& vRect)
    {
        for (auto& z : childs)
            z->to_rectangle(vRect);
        if (childs.size() > 0)
            return;
        auto s = sf::RectangleShape(max);
        s.setFillColor(sf::Color::Transparent);
        s.setOutlineThickness(1);
        s.setOutlineColor(sf::Color::Blue);
        auto genLine = [](sf::RectangleShape& s,
            float minx, float miny,
            float maxx, float maxy) ->sf::RectangleShape&
        {
            s.setPosition(sf::Vector2f(minx, miny));
            s.setSize(sf::Vector2f(maxx - minx, maxy - miny));
            return s;
        };
        vRect.emplace_back(genLine(s, min.x, min.y, max.x, max.y));
    }


    void zone::emplace_back(std::vector<physicsData>& vShapes)
    {
        auto isCircleOutOfZone = [](const physicsData& e,
            const sf::Vector2f& min,
            const sf::Vector2f& max) -> bool
        {
            auto x = e.position.x + e.radius;
            auto y = e.position.y + e.radius;
            return (
                max.x < x - e.radius ||
                min.x > x + e.radius ||
                max.y < y - e.radius ||
                min.y > y + e.radius);
        };
        data.reserve(data.size() < 64 ? data.size() : 64);
        for (auto& e : vShapes)
        {
            if (!isCircleOutOfZone(e, min, max))
                data.emplace_back(e);
        }
        if (bool isTooBig = data.size() >= 64)
        {
            auto hsx = (max.x - min.x) / 2;
            auto hsy = (max.y - min.y) / 2;
            if (minSize < hsx + hsx)
            {
                auto min00 = min;
                auto max00 = sf::Vector2f(min.x + hsx, min.y + hsy);
                auto min01 = sf::Vector2f(min.x + hsx, min.y);
                auto max01 = sf::Vector2f(max.x, min.y + hsy);
                auto min10 = sf::Vector2f(min.x, min.y + hsy);
                auto max10 = sf::Vector2f(min.x + hsx, max.y);
                auto min11 = max00;
                auto max11 = max;
                childs.emplace_back(std::make_unique<zone>(idEngine, data, vCollisions, min00, max00));
                childs.emplace_back(std::make_unique<zone>(idEngine, data, vCollisions, min01, max01));
                childs.emplace_back(std::make_unique<zone>(idEngine, data, vCollisions, min10, max10));
                childs.emplace_back(std::make_unique<zone>(idEngine, data, vCollisions, min11, max11));
            }
        }
        if (childs.size() == 0)
            id = idEngine++;
    }
}