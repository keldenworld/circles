#pragma once
#include <iostream>
#include <array>
#include <list>
#include "tools.h"

namespace ftw
{
    struct physicsData {
        sf::Vector2f position;
        float radius;
        physicsData(sf::Vector2f position, float radius) :
            position{ position }, radius{ radius } {}
    };

    class zone {
        int &idEngine;
        int id;
        sf::Vector2f min, max;
        std::vector<physicsData> data;
        std::vector<std::unique_ptr<zone>> childs;
        const int minSize{ 256 };
        void emplace_back(std::vector<physicsData>& vShapes)
        {
            auto isCircleOutOfZone = [](const physicsData& e,
                const sf::Vector2f& min,
                const sf::Vector2f& max) -> bool
            {
                return (
                    max.x < e.position.x + e.radius ||
                    min.x > e.position.x - e.radius ||
                    max.y < e.position.y + e.radius ||
                    min.y > e.position.y - e.radius);
            };
            data.reserve(data.size()<64? data.size():64);
            for (auto &e : vShapes)
            {
                if (!isCircleOutOfZone(e, min, max))
                    data.emplace_back(e);
            }
            if (bool isTooBig = data.size() >= 64)
            {
                auto hsx = (max.x - min.x) / 2;
                auto hsy = (max.y - min.y) / 2;
                if (minSize < hsx+hsx)
                {
                    auto min00 = min;
                    auto max00 = sf::Vector2f(min.x + hsx, min.y + hsy);
                    auto min01 = sf::Vector2f(min.x + hsx, min.y);
                    auto max01 = sf::Vector2f(max.x, min.y + hsy);
                    auto min10 = sf::Vector2f(min.x, min.y + hsy);
                    auto max10 = sf::Vector2f(min.x + hsx, max.y);
                    auto min11 = max00;
                    auto max11 = max;
                    childs.emplace_back(std::make_unique<zone>(idEngine, data, min00, max00));
                    childs.emplace_back(std::make_unique<zone>(idEngine, data, min01, max01));
                    childs.emplace_back(std::make_unique<zone>(idEngine, data, min10, max10));
                    childs.emplace_back(std::make_unique<zone>(idEngine, data, min11, max11));
                }
            }
        }
    public:
        zone(int &idEngine, std::vector<physicsData>& vDatas, 
            sf::Vector2f min = { 0, 0 },
            sf::Vector2f max = { MAXTerrain, MAXTerrain }) 
            : min{ min }, max{ max }, idEngine{ idEngine }, id{idEngine++}
        {
            emplace_back(vDatas);
        }
        void to_string(std::vector<std::tuple<std::string, float, float>>& 
            strzlog)
        {
            if (childs.size() == 0)
                strzlog.emplace_back(
                    std::tuple(
                        std::to_string(id) +" : "+ std::to_string(this->data.size()),
                        this->min.x, this->min.y));
            else
                for (auto& z : childs)
                    z->to_string(strzlog);
        }
        void to_circle(std::vector<sf::CircleShape>& circlesPerZone)
        {
            if (childs.size() == 0)
                for (auto& e : data)
                {
                    auto c = sf::CircleShape(1);
                    auto x = e.position.x + e.radius;
                    auto y = e.position.y + e.radius;
                    c.setPosition(x,y);
                    if (id == 4) //debug
                        circlesPerZone.emplace_back(c);
                }
            else
                for (auto& z : childs)
                    z->to_circle(circlesPerZone);
        }
        void to_rectangle(std::vector<sf::RectangleShape>& vRect)
        {
            for (auto &z : childs)
                z->to_rectangle(vRect);
            if (childs.size() > 0) 
                return ;
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
        size_t size() 
        {
            return data.size();
            //size_t n = 0;
            //for (auto &e : childs)
            //    n += e->size();
            //return n;
        }
    };

    class world_basedonvector
    {
        struct data
        {
            sf::CircleShape shape;
            std::chrono::time_point<std::chrono::system_clock>
                dt{ std::chrono::system_clock::now() };
            sf::Vector2f speed;
            sf::Vector2f force;
            sf::CircleShape zoomedShape;
            float friction;
            float mass;
            data(sf::CircleShape shape, sf::Vector2f speed, sf::Vector2f force, float friction, float mass) :
                shape{ shape }, speed{ speed }, force{ force }, friction{ friction }, mass{ mass }, zoomedShape{ shape } {}
        };
        std::vector<data> physics;
        std::vector<physicsData> currentPhysics;
        std::vector<sf::CircleShape> drawingPhysics;
        sf::Vector2f deltaPos{ 0,0 };
        float zoomVal{ 1 };
        bool logFlag{ true };
        bool circleFlag{ true };
        bool gridFlag{ true };
        void zoomUpdate();
        void initload();
        void log(std::string& txt);
        void setDeltaPos(int x, int y);
        void draw(sf::RenderWindow& window);
        void updateDraw(std::map<std::string, std::tuple<sf::Color, float>>& timers);
        void zoomUp() { zoomVal += 0.2f; zoomUpdate(); }
        void zoomDown() { zoomVal -= 0.2f; zoomUpdate(); }
        void update();
        void deletePhysics(size_t iemElement);
    public:
        void loop();
    };

}