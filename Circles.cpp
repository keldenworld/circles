//https://github.com/keldenworld/circles
#include <iostream>
#include <chrono>
#include <ctime>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>
#include "circles.h"
#include "tools.h"

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
        sf::Vector2f deltaPos {0,0};
        float zoom{ 1 };
        struct ZoomType { int pos; };
        const ZoomType zoomDown{ 1 };
        const ZoomType zoomUp{ 0 };
    public:
        std::string type() { return "WorldVector"; }
        void initload()
        {
            sf::CircleShape shape(100);
            shape.setPointCount(MAXPoint);
            shape.setFillColor(sf::Color::Transparent);
            for (int i = 0; i < MAXOBJ; i++)
            {
                shape.setPosition(static_cast<float>(rand() % MAXTerrain - 255) + 255, static_cast<float>(rand() % MAXTerrain - 255) + 255);
                shape.setRadius(static_cast<float>(rand() % (64 + 3) - 3));
                shape.setOutlineColor(sf::Color(rand() % 255, rand() % 255, rand() % 255));
                shape.setOutlineThickness(1);
                currentPhysics.emplace_back(sf::Vector2f(shape.getPosition().x, shape.getPosition().y));
                physics.emplace_back(data(shape,
                    sf::Vector2f(static_cast<float>(rand() % 32) - 16.0f, static_cast<float>(rand() % 32) - 16.0f),
                    sf::Vector2f(static_cast<float>(rand() % 32) - 16.0f, static_cast<float>(rand() % 32) - 16.0f),
                    static_cast<float>(rand() % 32),
                    static_cast<float>(rand() % 64)));
            }
        }
        void log(std::string& txt)
        { 
            auto elem = physics.size();
            auto density = (float)elem / (float)MAXTerrain;
            txt += "Nb Elements: "+ std::to_string(elem) + "\n" +
                "Delta Pos : " + std::to_string(deltaPos.x) + "/" + std::to_string(deltaPos.y) + "\n" +
                "Zoom : " + std::to_string(zoom) + "\n" +
                "Drawned Elements : " + std::to_string(drawingPhysics.size()) + "\n" +
                "Density : " + std::to_string(density) + "\n" ;
        }
        void setDeltaPos(int x, int y)
        {
            deltaPos.x += x;
            deltaPos.y += y;
            if (deltaPos.x > 0) deltaPos.x = 0;
            if (deltaPos.y > 0) deltaPos.y = 0;
            if (deltaPos.x < - MAXTerrain + MAXScreen_X) deltaPos.x = - MAXTerrain + MAXScreen_X;
            if (deltaPos.y < - MAXTerrain + MAXScreen_Y) deltaPos.y = - MAXTerrain + MAXScreen_Y;
        }
        void draw(sf::RenderWindow& window)
        {
            for (auto e : drawingPhysics)
                window.draw(e);
        }
        void updateDraw(std::map<std::string, std::tuple<sf::Color, float>> &timers)
        {
            auto Orange = sf::Color(0xFF, 0xA5, 0x00);
            sf::Vector2f v2f;
            sf::CircleShape shape;
            size_t n = 0;
            for (auto i = 0; i < physics.size(); i++)
            {
                    ftw::timethat timet(timers, "update draw [compute position] - ORANGE", Orange);
                    shape = physics[i].zoomedShape;
                    v2f = sf::Vector2f(
                        (this->currentPhysics[i].x + deltaPos.x) * zoom,
                        (this->currentPhysics[i].y + deltaPos.y) * zoom
                    );
                    shape.setPosition(v2f);
                    if (n < drawingPhysics.size())
                        drawingPhysics[n] = shape;
                    else
                        drawingPhysics.emplace_back(shape);
                    n++;
            }
            if (bool containOldDataAtTheEndOfDrawingVector = (n != drawingPhysics.size()))
                drawingPhysics.erase(drawingPhysics.begin() + n, drawingPhysics.end());
        }
        void setZoom(ZoomType zt)
        {
            zoom += (zt.pos == 0) ? 0.2f : -0.2f;
            if (zoom < 0.2f) zoom = 0.2f;
            for (auto e : this->physics)
                e.zoomedShape.setRadius(e.shape.getRadius() * zoom);
        }
        void loop()
        {
            sf::RenderWindow window(sf::VideoMode(MAXScreen_X, MAXScreen_Y), "Hello Kelden World with SFML!");
            window.setVerticalSyncEnabled(false);
        
            sf::Clock clock;
            std::string tmpstr;
            sf::Text text2prt;
            text2prt.setCharacterSize(21);
            text2prt.setFillColor(sf::Color::White);
            sf::Font font;
            if (!font.loadFromFile("monofonto.ttf"))
                return;
            text2prt.setFont(font);

            std::map<std::string, std::tuple<sf::Color, float>> timers;
            auto myfps = ftw::FPS();
            while (window.isOpen())
            {
                {
                    ftw::timethat timet(timers, "update physics - CYAN", sf::Color::Cyan);
                    this->update();
                }
                {
                    ftw::timethat timet(timers, "event loop + window.clear - GREEN", sf::Color::Green);
                    sf::Event event;
                    while (window.pollEvent(event))
                    {
                        if (event.type == sf::Event::Closed)
                            window.close();
                        else if (event.type == sf::Event::KeyPressed)
                            if (event.key.code == sf::Keyboard::Escape)
                                window.close();
                            else if (event.key.code == sf::Keyboard::Space)
                                initload();
                            else if (event.key.code == sf::Keyboard::Up)
                                setDeltaPos(0, 30);
                            else if (event.key.code == sf::Keyboard::Down)
                                setDeltaPos(0, -30);
                            else if (event.key.code == sf::Keyboard::Right)
                                setDeltaPos(-30, 0);
                            else if (event.key.code == sf::Keyboard::Left)
                                setDeltaPos(30, 0);
                            else if (event.key.code == sf::Keyboard::Subtract)
                                setZoom(zoomDown);
                            else if (event.key.code == sf::Keyboard::Add)
                                setZoom(zoomUp);
                    }
                    window.clear();
                }
                updateDraw(timers);
                {
                    ftw::timethat timet(timers, "windows.draw circles - BLUE", sf::Color::Blue);
                    this->draw(window);
                }
                {
                    ftw::timethat timet(timers, "log Rectangle - YELLOW", sf::Color::Yellow);
                    for (auto r : timet.to_rectangle())
                        window.draw(r);
                }
                {
                    ftw::timethat timet(timers, "log text - WHITE", sf::Color::White);
                    log(tmpstr);
                    tmpstr += timet.to_string();
                }
                {
                    ftw::timethat timet(timers, "log FPS - BLACK", sf::Color::Black);
                    myfps.update();
                    tmpstr +="\nFPS (basics " + this->type() + ") : " + std::to_string(myfps.get()) + "\n";
                }
                {
                    auto LightPink = sf::Color(0xFF, 0xB6, 0xC1);
                    ftw::timethat timet(timers, "prepare text log - LIGHTPINK", LightPink);
                    text2prt.setString(tmpstr);
                    tmpstr = "";
                }
                {
                    auto Purple = sf::Color(0x80, 0x00, 0x80);
                    ftw::timethat timet(timers, "print text log - PURPLE", Purple);
                    window.draw(text2prt);
                }
                {
                    ftw::timethat timet(timers, "Windows.Display - MAGENTA", sf::Color::Magenta);
                    window.display();
                }
            }
        }
        void update()
        {
            auto currentdt = std::chrono::system_clock::now();
            for (auto ii = physics.size(); ii > 0 ; ii--)
            {
                int i = (int)ii - 1; // dirty, for deletePhysics algorithme
                //auto ray = physics[i].shape.getRadius();
                auto mass = physics[i].mass;
                auto dtm = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds> (currentdt - physics[i].dt).count());
                float friction = physics[i].friction;
                auto force = physics[i].force;
                auto speed = physics[i].speed;
                float x = physics[i].shape.getPosition().x;
                float y = physics[i].shape.getPosition().y;
                float t_setdtm = dtm / 1000.0f;
                float e_setdtm = exp(-friction / mass * t_setdtm);

                auto equaDiff = [](float force, float friction, float dtm, float speed, float mass, float e_setdtm, float x) -> float
                {return ((force / friction) * dtm + (force / friction - speed * 100) * mass / friction * (e_setdtm - 1) + x * 100) / 100; };
                auto newx = equaDiff(force.x, friction, dtm, speed.x, mass, e_setdtm, x);
                auto newy = equaDiff(force.y, friction, dtm, speed.y, mass, e_setdtm, y);

                currentPhysics[i] = sf::Vector2f(newx * zoom, newy * zoom);

                if (bool isOutOfTerrain = newx<0 || newx>MAXTerrain || newy<0 || newy>MAXTerrain)
                    deletePhysics(i);
            }
        }
        //replace with the last element in the vector (witch should be hopefully processed...)
        void deletePhysics(size_t iemElement)
        {
            if (currentPhysics.size() - 1 == iemElement) return; //already the last one
            {
                currentPhysics[iemElement] = currentPhysics.back();
                drawingPhysics[iemElement] = drawingPhysics.back();
                physics[iemElement] = physics.back();
            }
            currentPhysics.pop_back();
            drawingPhysics.pop_back();
            physics.pop_back();
        }
    };
}

template <typename myContainer> 
int executeWorld(myContainer* circles)
{
    sf::RenderWindow window(sf::VideoMode(1024, 768), "Hello Kelden World with SFML!");
    window.setVerticalSyncEnabled(false);

    sf::CircleShape shape(100);
    shape.setPointCount(MAXPoint);
    shape.setFillColor(sf::Color::Transparent);
    for (int i = 0; i < MAXOBJ; i++)
    {
        shape.setPosition(static_cast<float>(rand() % 1024 - 255) + 255, static_cast<float>(rand() % 728 - 255) + 255);
        shape.setRadius(static_cast<float>(rand() % (255 + 5) - 5));
        shape.setOutlineColor(sf::Color(rand() % 255, rand() % 255, rand() % 255));
        shape.setOutlineThickness(1);
        circles->add(shape);
    }

    sf::Clock clock;
    sf::Text text;
    text.setCharacterSize(32);
    text.setFillColor(sf::Color::White);
    sf::Font font;
    if (!font.loadFromFile("monofonto.ttf"))
        return -1;
    text.setFont(font);

    std::map<std::string, std::tuple<sf::Color, float>> timers;
    auto myfps = ftw::FPS();
    while (window.isOpen())
    {
        ftw::timethat timet(timers, "chrono Frametime (n-1) - RED", sf::Color::Red);
        {
            ftw::timethat timet(timers, "event loop + window.clear - GREEN", sf::Color::Green);
            sf::Event event;
            while (window.pollEvent(event))
                if (event.type == sf::Event::Closed)
                    window.close();
                else if (event.type == sf::Event::KeyPressed)
                    if (event.key.code == sf::Keyboard::Escape)
                        window.close();
            window.clear();
        }
        {
            ftw::timethat timet(timers, "windows.draw circles - BLUE", sf::Color::Blue);
            for (auto& s : *circles)
                window.draw(s);
        }
        {
            ftw::timethat timet(timers, "log text and rectangles - GREEN", sf::Color::Green);
            for (auto r : timet.to_rectangle())
                window.draw(r);
            myfps.update();
            text.setString("\nFPS (basics " + circles->type() + ") : " + 
                std::to_string(myfps.get()) + "\n"
                + timet.to_string()
            );
            window.draw(text);
        }
        {
            ftw::timethat timet(timers, "Windows.Display - MAGENTA", sf::Color::Magenta);
            window.display();
        }
    }
    return 0;
}

int main()
{
    {
        auto world = ftw::world_basedonvector();
        world.loop();
    }
    {
        auto circles = std::make_unique<ftw::container_basedonvector<sf::CircleShape>>();
        executeWorld<ftw::container_basedonvector<sf::CircleShape>>(circles.get());
    }
    {
        auto circles = std::make_unique<ftw::container_basedonarray<sf::CircleShape>>();
        executeWorld<ftw::container_basedonarray<sf::CircleShape>>(circles.get());
    }
    {
        auto circles = std::make_unique<ftw::container_basedonlist<sf::CircleShape>>();
        executeWorld<ftw::container_basedonlist<sf::CircleShape>>(circles.get());
    }
    return 0;
}
