#pragma once
#include <iostream>
#include <array>
#include <list>
#include "tools.h"
#include "assert.h"
#include "zone.h"

namespace ftw
{
    class world_basedonvector
    {
        struct data
        {
            sf::CircleShape shape;
            std::chrono::time_point<std::chrono::system_clock>
                dt{ std::chrono::system_clock::now() };
            data(sf::CircleShape shape) :
                shape{ shape } {}
        };
        std::vector<data> physics;
        std::vector<std::vector<sf::Vector2f>> deltaSpeeds;
        std::vector<physicsData> currentPhysics;
        std::vector<sf::CircleShape> drawingPhysics;
        sf::Vector2f deltaPos{ 90,0 };
        float zoomVal{ 0.4f };
        bool logFlag{ false };
        bool objectFlag{ true };
        bool gridFlag{ false };
        bool collisionFlag{ false };
        void zoomUpdate();
        void initload()
        {
            sf::CircleShape shape(100);
            shape.setPointCount(MAXPoint);
            shape.setFillColor(sf::Color::Transparent);
            shape.setOutlineThickness(1);
            auto currentSize = physics.size();
            for (int i = 0; i < MAXOBJ / 10; i++)
                //sf::CircleShape& shape, sf::Vector2f position,
                //float radius, sf::Vector2f speed, sf::Vector2f force, float mass
                setCircle(shape,
                    { static_cast<float>(rand() % MAXTerrain / 2 + MAXTerrain / 4),
                    static_cast<float>(rand() % MAXTerrain / 2 + MAXTerrain / 4) },
                    static_cast<float>(rand() % (61 + 3)),
                    { static_cast<float>(rand() % 2000) - 1000,//speed
                    static_cast<float>(rand() % 2000) - 1000 },
                    {},//{ static_cast<float>(rand() % 1'000) - 500, //force
                    //static_cast<float>(rand() % 1'000) - 500 }, 
                    static_cast<float>(rand() % 500) //mass
                );
            zoomUpdate();
        }
        void setCircle (sf::CircleShape& shape, sf::Vector2f position,
            float radius, sf::Vector2f speed, sf::Vector2f force, float mass,
            sf::Color col = sf::Color( rand() % 224 + 31, rand() % 224 + 31, rand() % 224 + 31) ) {
                shape.setPosition(position);
                shape.setRadius(radius);
                shape.setOutlineColor(col);
                auto currentSize = physics.size();
                physics.emplace_back(data(shape));
                currentPhysics.emplace_back(physicsData(
                    currentSize,
                    sf::Vector2f(
                        shape.getPosition().x,          //pos
                        shape.getPosition().y),
                    radius,                         //radius - 1
                    speed * 1000.0f,
                    force,
                    static_cast<float>(rand() % 32) / 10'000,   //friction
                    mass));         //mass - 500
                deltaSpeeds.emplace_back(std::vector<sf::Vector2f>());
        };
        void initTest1()
        {
            sf::CircleShape shape(100);
            shape.setPointCount(MAXPoint);
            shape.setFillColor(sf::Color::Transparent);
            shape.setOutlineThickness(1);
            setCircle(shape, { 100,100 }, 100, { 100,0 }, { 100,0 }, 10);
            setCircle(shape, { 500,100 }, 100, { 0,0 }, { 0,0 }, 10);
            setCircle(shape, { 800,100 }, 100, { 0,0 }, { 0,0 }, 10);
            zoomUpdate();
        }
        void log(std::string& txt);
        void setDeltaPos(int x, int y);
        void draw(sf::RenderWindow& window);
        void updateDraw(std::map<std::string, std::tuple<sf::Color, float>>& timers);
        void zoomUp() { zoomVal += 0.2f; zoomUpdate(); }
        void zoomDown() { zoomVal -= 0.2f; zoomUpdate(); }
        void updatePhysics();
        void deletePhysics(size_t iemElement);
        void updateCollisions(std::vector<std::vector<size_t>>& vCollisions)
        {
            auto capSpeed = [](sf::Vector2f& spd1, sf::Vector2f& spd2)
            {
                auto speed = 
                    sqrt(spd1.x * spd1.x + spd1.y * spd1.y) +
                    sqrt(spd2.x * spd2.x + spd2.y * spd2.y);
                auto oldsp1 = spd1;
                auto oldsp2 = spd2;
                auto localspeed1 = spd1.x * spd1.x + spd1.y * spd1.y;
                if (localspeed1 > speed) 
                    spd1 *= sqrt(speed * speed / localspeed1);
                auto localspeed2 = spd2.x * spd2.x + spd2.y * spd2.y;
                if (localspeed2 > speed) 
                    spd2 *= sqrt(speed * speed / localspeed2);
                int i=0;
            };
            std::cout << "\n>>";
            for (auto i = 0; i < physics.size(); i++)
            {
                auto c1 = currentPhysics[i];
                auto s1 = physics[i].shape;
                s1.setPosition(currentPhysics[i].position);
                physics[i] = data(s1);
                for (auto& e : vCollisions[i])
                {
                    auto s2 = physics[e].shape;
                    s2.setPosition(currentPhysics[e].position);
                    physics[e] = data(s2);
                    auto c2 = currentPhysics[e];
                    auto dist = c2.position - c1.position;
                    float norm = sqrt(dist.x * dist.x + dist.y * dist.y);
                    dist.x = (dist.x == 0) ? 1.0f : dist.x /= norm;
                    dist.y = (dist.y == 0) ? 1.0f : dist.y /= norm;
                    float energyEntrance = 
                        c1.mass * sqrt(c1.speed.x * c1.speed.x + c1.speed.y * c1.speed.y) + 
                        c2.mass * sqrt(c2.speed.x * c2.speed.x + c2.speed.y * c2.speed.y);
                    if (energyEntrance <= 0.0f) energyEntrance = 1.0f;
                    if (energyEntrance > 2.0f) energyEntrance = 2.0f;
                    float spd =
                        sqrt(c1.speed.x * c1.speed.x + c1.speed.y * c1.speed.y) +
                        sqrt(c2.speed.x * c2.speed.x + c2.speed.y * c2.speed.y);
                    float absCoef = 0.1f;
                    float c1k2 = 2 * c1.mass / (c1.mass + c2.mass);
                    c1.speed = (-spd * c1k2 * dist + c1.speed) * absCoef;
                    //assert(!isnan(c1.speed.x));
                    //assert(c1.speed.x < 10'000);
                    float c2k2 = 2 * c2.mass / (c1.mass + c2.mass);
                    c2.speed = (-spd * c1k2 * dist + c2.speed) * absCoef;
                    assert(!isnan(c2.speed.x));
                    if (i == 44) 
                        std::cout << c1.speed.x << "("<<c1.radius<<")["<<i<<"/"<<e<<"], ";
                    //assert(c2.speed.x < 10'000);
                    float energyExit = c1.mass *
                        sqrt(c1.speed.x * c1.speed.x + c1.speed.y * c1.speed.y) +
                        c2.mass * 
                        sqrt(c2.speed.x * c2.speed.x + c2.speed.y * c2.speed.y);
                    if (energyExit <= 0.0f) energyExit = 1.0f;
                    if (energyExit > 2.0f ) energyExit = 2.0f;
                    assert(!isinf(energyExit));
                    float energyAbsorbedCoef = energyEntrance / energyExit * absCoef;
                    c1.speed = c1.speed * energyAbsorbedCoef;
                    c2.speed = c2.speed * energyAbsorbedCoef;
                    //capSpeed(c1.speed, c2.speed);
                    auto nspd1 = currentPhysics[i].speed - c1.speed;
                    auto nspd2 = currentPhysics[e].speed - c2.speed;
                    assert(!isinf(nspd1.x));
                    //if (nspd1.x < 1000 && nspd2.x < 1000)
                    {
                        deltaSpeeds[i].emplace_back(nspd1);
                        deltaSpeeds[e].emplace_back(nspd2);
                    }
                }
            }
        }
    public:
        void loop();
    };

}