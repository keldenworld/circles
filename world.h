#pragma once
#include <iostream>
#include <array>
#include <list>
#include "tools.h"
#include "assert.h"
#include "zone.h"
#include "box2d/box2d.h"

namespace ftw
{
    class world
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
        b2World b2World{ {0.0f, -9.81f} };
        std::vector<b2Body*> vBodies;
        void zoomUpdate();
        void initload();
        void setCircle (sf::CircleShape& shape, sf::Vector2f position,
            float radius, sf::Vector2f speed, sf::Vector2f force, float mass,
            sf::Color col = sf::Color(rand() % 224 + 31, rand() % 224 + 31, rand() % 224 + 31));
        void initTest1();
        void log(std::string& txt);
        void setDeltaPos(int x, int y);
        void draw(sf::RenderWindow& window);
        void updateDraw(std::map<std::string, std::tuple<std::string, sf::Color, float>>& timers);
        void updatePhysics();
        void deletePhysics(size_t iemElement);
        void updateCollisions(std::vector<std::vector<size_t>>& vCollisions);
public:
        void loop();
        void box2DGround();
        void box2DSquare();
        void box2DPyramid();
    };

}