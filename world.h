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
            data(sf::CircleShape shape) :
                shape{ shape } {}
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
        void updateCollisions(std::vector<std::vector<size_t>>& vCollisions)
        {
            for (auto& e : vCollisions)
                ;
        }
    public:
        void loop();
    };

}