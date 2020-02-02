#pragma once

namespace ftw
{
    struct physicsData {
        size_t currentId;
        sf::Vector2f position;
        float radius;
        physicsData(size_t currentId, sf::Vector2f position, float radius) :
            position{ position }, radius{ radius }, currentId{ currentId }{}
    };

    class zone {
        int& idEngine;
        int id{ -1 };
        sf::Vector2f min, max;
        std::vector<size_t> collisionsWith;
        std::vector<physicsData> data;
        std::vector<std::unique_ptr<zone>> childs;
        std::vector<std::vector<size_t>>& vCollisions;
        const int minSize{ 256 };
        void emplace_back(std::vector<physicsData>& vShapes);
    public:
        size_t size() { return data.size(); }
        zone(int& idEngine, std::vector<physicsData>& vDatas,
            std::vector<std::vector<size_t>>& vCollisions,
            sf::Vector2f min = { 0, 0 },
            sf::Vector2f max = { MAXTerrain, MAXTerrain });
        void to_string(std::vector<std::tuple<std::string, float, float>>& strzlog);
        void to_circle(std::vector<sf::CircleShape>& circlesPerZone);
        void to_rectangle(std::vector<sf::RectangleShape>& vRect);
    };
}