#include "world.h"
#include "assert.h"

namespace ftw
{
    void world_basedonvector::initload()
    {
        sf::CircleShape shape(100);
        shape.setPointCount(MAXPoint);
        shape.setFillColor(sf::Color::Transparent);
        shape.setOutlineThickness(1);
        for (int i = 0; i < MAXOBJ/10; i++)
        {
            shape.setPosition(
                static_cast<float>(rand() % MAXTerrain/2 + MAXTerrain / 4),
                static_cast<float>(rand() % MAXTerrain/2 + MAXTerrain / 4));
            auto radius = static_cast<float>(rand() % (64 + 3) - 3);
            shape.setRadius(radius);
            shape.setOutlineColor(sf::Color(rand() % 255, rand() % 255, rand() % 255));
            currentPhysics.emplace_back(physicsData(sf::Vector2f(
                    shape.getPosition().x, 
                    shape.getPosition().y), radius));
            physics.emplace_back(data(shape,
                sf::Vector2f(static_cast<float>(rand() % 512) - 256.0f, static_cast<float>(rand() % 512) - 256.0f),
                sf::Vector2f(static_cast<float>(rand() % 32) - 16.0f, static_cast<float>(rand() % 32) - 16.0f),
                static_cast<float>(rand() % 32),
                static_cast<float>(rand() % 64)));
        }
        zoomUpdate();
    }
    void world_basedonvector::log(std::string& txt)
    {
        auto elem = physics.size();
        auto density = (float)elem / (float)MAXTerrain;
        txt += "Nb Elements: " + std::to_string(elem) + "\n" +
            "Delta Pos : "
            + std::to_string(static_cast<int>(deltaPos.x)) + "/"
            + std::to_string(static_cast<int>(deltaPos.y)) + "\n" +
            "Zoom : " + std::to_string(zoomVal) + "\n" +
            "Drawned Elements : " + std::to_string(drawingPhysics.size()) + "\n" +
            "Density : " + std::to_string(density) + "\n";
    }
    void world_basedonvector::setDeltaPos(int x, int y)
    {
        deltaPos.x += x;
        deltaPos.y += y;
        float limitX = (float)(MAXTerrain - MAXScreen_X);
        float limitY = (float)(MAXTerrain - MAXScreen_Y);
        if (deltaPos.x > limitX) deltaPos.x = limitX;
        if (deltaPos.y > limitY) deltaPos.y = limitY;
        if (deltaPos.x < -limitX) deltaPos.x = -limitX;
        if (deltaPos.y < -limitY) deltaPos.y = -limitY;
    }

    void world_basedonvector::draw(sf::RenderWindow& window) 
    {
        for (const auto e : drawingPhysics)
            window.draw(e);
    }

    void world_basedonvector::updateDraw(std::map<std::string, std::tuple<sf::Color, float>>& timers)
    {
        auto Orange = sf::Color(0xFF, 0xA5, 0x00);
        auto DeepSkyblue = sf::Color(0x00, 0xBF, 0xFF);
        sf::Vector2f v2f;
        sf::CircleShape shape;
        size_t n = 0;
        sf::Vector3f deltaPos2 = sf::Vector3f(deltaPos.x, deltaPos.y, 1);
        for (auto i = 0; i < physics.size(); i++)
        {
            {
                ftw::timethat timet(timers, "update draw [compute position] - ORANGE", Orange);
                v2f = this->currentPhysics[i].position;
                v2f *= zoomVal;
                v2f += deltaPos;
            }
            if (bool isVisible = (v2f.x < MAXScreen_X&& v2f.y < MAXScreen_Y))
            {
                ftw::timethat timet(timers, "update draw [insert into drawingPhysics] - DEEPSKYBLUE", DeepSkyblue);
                shape = physics[i].zoomedShape;
                shape.setPosition(v2f);
                if (n < drawingPhysics.size())
                    drawingPhysics[n] = shape;
                else
                    drawingPhysics.emplace_back(shape);
                n++;
            }
        }
        {
            auto DarkViolet = sf::Color(0x94, 0x00, 0xD3);
            ftw::timethat timet(timers, "update draw [clean drawing vector] - DARKVIOLET", DarkViolet);
            if (bool containOldDataAtTheEndOfDrawingVector = (n != drawingPhysics.size()))
                drawingPhysics.erase(drawingPhysics.begin() + n, drawingPhysics.end());
        }
    }
    void world_basedonvector::zoomUpdate()
    {
        if (zoomVal < 0.2f) zoomVal = 0.2f;
        for (auto& e : this->physics)
            e.zoomedShape.setRadius(e.shape.getRadius() * zoomVal);
    }
    void world_basedonvector::loop()
    {
        sf::RenderWindow window(sf::VideoMode(MAXScreen_X, MAXScreen_Y), "Hello Kelden World with SFML!");
        window.setVerticalSyncEnabled(false);

        sf::Clock clock;
        std::string tmpstr;
        sf::Font font;
        if (!font.loadFromFile("monofonto.ttf")) return;
        sf::Text text2prt;
        text2prt.setCharacterSize(21);
        text2prt.setFont(font);
        sf::Text zoneText2prt;
        zoneText2prt.setCharacterSize(21);
        zoneText2prt.setFont(font);
        zoneText2prt.setFillColor(sf::Color::Green);

        std::vector<sf::RectangleShape> zones;
        std::map<std::string, std::tuple<sf::Color, float>> timers;
        auto myfps = ftw::FPS();
        while (window.isOpen())
        {
            tmpstr = "";
            {
                ftw::timethat timet(timers, "update physics [equadiff] - CYAN", sf::Color::Cyan);
                this->update();
            }
            {
                ftw::timethat timet(timers, "event loop + window.clear - GREEN", sf::Color::Green);
                sf::Event event;
                tmpstr += "Commands : +/-, up/down, l, c, g, space, esc \n";
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
                            zoomDown();
                        else if (event.key.code == sf::Keyboard::Add)
                            zoomUp();
                        else if (event.key.code == sf::Keyboard::L)
                            logFlag = !logFlag;
                        else if (event.key.code == sf::Keyboard::C)
                            circleFlag = !circleFlag;
                        else if (event.key.code == sf::Keyboard::G)
                            gridFlag = !gridFlag;
                }
                window.clear();
            }
            updateDraw(timers);
            {
                ftw::timethat timet(timers, "windows.draw circles - BLUE", sf::Color::Blue);
                if (circleFlag) this->draw(window);
            }
            {
                ftw::timethat timet(timers, "log Rectangle - YELLOW", sf::Color::Yellow);
                for (auto &r : timet.to_rectangle()) 
                {
                    auto zoomedShape = r;
                    auto thick = zoomedShape.getOutlineThickness();
                    zoomedShape.setPosition(sf::Vector2f(
                        r.getPosition().x * zoomVal,
                        r.getPosition().y * zoomVal));
                    zoomedShape.setSize(sf::Vector2f(
                        r.getSize().x * zoomVal,
                        r.getSize().y * zoomVal));
                    window.draw(zoomedShape);
                }
            }
            {
                ftw::timethat timet(timers, "log text - WHITE", sf::Color::White);
                log(tmpstr);
                tmpstr += timet.to_string();
            }
            if (logFlag)
            {
                {
                    ftw::timethat timet(timers, "log FPS - BLACK", sf::Color::Black);
                    myfps.update();
                    auto fpsval = myfps.get();
                    tmpstr += "\nFPS (basics World Vector) : " + 
                        std::to_string(fpsval) + "\n";
                    text2prt.setFillColor((fpsval > 60 ? sf::Color::Green : sf::Color::White));
                }
                {
                    auto LightPink = sf::Color(0xFF, 0xB6, 0xC1);
                    ftw::timethat timet(timers, "prepare text log - LIGHTPINK", LightPink);
                    text2prt.setString("\n\n" + tmpstr);
                }
                {
                    auto Purple = sf::Color(0x80, 0x00, 0x80);
                    ftw::timethat timet(timers, "print text log - PURPLE", Purple);
                    window.draw(text2prt);
                }
            }
            {
                auto DimGray = sf::Color(0x69, 0x69, 0x69);
                ftw::timethat timet(timers, "Generate Zones - DIMGRAY", DimGray);
                int idEngine = 0; 
                zone headZone{ idEngine, currentPhysics };
                std::vector<std::tuple<std::string, float, float>> strzlog; //s,x,y
                if (gridFlag)
                {
                    headZone.to_string(strzlog);
                    for (auto& e : strzlog)
                    {
                        zoneText2prt.setString(std::get<0>(e));
                        zoneText2prt.setPosition(
                            std::get<1>(e) * zoomVal + deltaPos.x,
                            std::get<2>(e) * zoomVal + deltaPos.y);
                        window.draw(zoneText2prt);
                    }
                    std::vector<sf::CircleShape> circlesPerZone;
                    headZone.to_circle(circlesPerZone);
                    for (auto& e : circlesPerZone)
                    {
                        e.setPosition(sf::Vector2f(
                            e.getPosition().x * zoomVal + deltaPos.x, 
                            e.getPosition().y * zoomVal + deltaPos.y));
                        window.draw(e);
                    }
                    headZone.to_rectangle(zones);
                    for (const auto& e : zones)
                    {
                        auto zoomedShape = e;
                        zoomedShape.setPosition(sf::Vector2f(
                            e.getPosition().x * zoomVal + deltaPos.x,
                            e.getPosition().y * zoomVal + deltaPos.y));
                        zoomedShape.setSize(sf::Vector2f(
                            e.getSize().x * zoomVal,
                            e.getSize().y * zoomVal));
                        window.draw(zoomedShape);
                    }
                }
                zones.clear();
            }
            ftw::timethat timet(timers, "Windows.Display - MAGENTA", sf::Color::Magenta);
            window.display();
        }
    }
    void world_basedonvector::update()
    {
        auto currentdt = std::chrono::system_clock::now();
        for (auto ii = physics.size(); ii > 0; ii--)
        {
            int i = (int)ii - 1; // dirty, linked to deletePhysics algorithm
            auto mass = physics[i].mass;
            auto dtm = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds> (currentdt - physics[i].dt).count());
            float friction = physics[i].friction;
            if (friction == 0) friction = 0.000001f;
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
            bool isOutOfTerrain;
            if (isOutOfTerrain = newx<0 || newx>MAXTerrain || newy<0 || newy>MAXTerrain)
                deletePhysics(i);
            else 
            {
                currentPhysics[i].position = sf::Vector2f(newx, newy);
                assert(newx > 0);
            }
        }
    }
    //replace with the last element in the vector (witch should be hopefully already processed...)
    void world_basedonvector::deletePhysics(size_t iemElement)
    {
        if (currentPhysics.size() - 1 != iemElement)  //not the last one
        {
            currentPhysics[iemElement] = currentPhysics.back();
            physics[iemElement] = physics.back();
        }
        currentPhysics.pop_back();
        physics.pop_back();
    }
}