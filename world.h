#pragma once
#include <iostream>
#include <array>
#include <list>
#include "tools.h"
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
            sf::Vector2f speed;
            sf::Vector2f force;
            sf::CircleShape zoomedShape;
            float friction;
            float mass;
            data(sf::CircleShape shape, sf::Vector2f speed, sf::Vector2f force, 
                float friction, float mass) :
                shape{ shape }, speed{ speed }, force{ force }, 
                friction{ friction }, mass{ mass }, zoomedShape{ shape } {}
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