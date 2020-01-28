#pragma once
#include <array>
#include <list>
#include "tools.h"

const unsigned int MAXOBJ = 1024;
const unsigned int MAXPoint = 16;
const int MAXTerrain = 1024 * 2;
const int MINScreen_X = 0;
const int MINScreen_Y = 0;
const int MAXScreen_X = 1024;
const int MAXScreen_Y = 768;

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
            data(sf::CircleShape shape, sf::Vector2f speed, sf::Vector2f force, float friction, float mass) :
                shape{ shape }, speed{ speed }, force{ force }, friction{ friction }, mass{ mass }, zoomedShape{ shape } {}
        };
        std::vector<data> physics;
        std::vector<sf::Vector2f> currentPhysics;
        std::vector<sf::CircleShape> drawingPhysics;
        sf::Vector2f deltaPos{ 0,0 };
        float zoom{ 1 };
        bool logFlag{ true };
    public:
        void initload();
        void log(std::string& txt);
        void setDeltaPos(int x, int y);
        void draw(sf::RenderWindow& window);
        void updateDraw(std::map<std::string, std::tuple<sf::Color, float>>& timers);
        void zoomUp() { zoom += 0.2f; zoomUpdate(); }
        void zoomDown() { zoom -= 0.2f; zoomUpdate(); }
        void zoomUpdate();
        void loop();
        void update();
        void deletePhysics(size_t iemElement);
    };

}