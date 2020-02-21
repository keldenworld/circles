#include "world.h"
#include "zone.h"
#include "assert.h"

namespace ftw
{
    void world_basedonvector::log(std::string& txt)
    {
        auto elem = physics.size();
        auto density = (float)elem / (float)MAXTerrain;
        txt += "Nb Elements: " + std::to_string(elem) + "\n" +
            "Delta Pos : "
            + std::to_string(static_cast<int>(deltaPos.x)) + "/"
            + std::to_string(static_cast<int>(deltaPos.y)) + "\n" +
            "Zoom : " + std::to_string(zoomVal) + "\n" +
            "CollisionFlag : " + (collisionFlag ? "TRUE" : "FALSE") + "\n" +
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
                shape = physics[i].shape;
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
        for (auto i = 0 ; i< physics.size() ; i++)
            physics[i].shape.setRadius(currentPhysics[i].radius * zoomVal);
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

        std::map<std::string, std::tuple<sf::Color, float>> timers;
        auto myfps = ftw::FPS();
        while (window.isOpen())
        {
            tmpstr = "";
            {
                ftw::timethat timet(timers, "update physics [equadiff] - CYAN", sf::Color::Cyan);
                this->updatePhysics();
            }
            {
                ftw::timethat timet(timers, "event loop + window.clear - GREEN", sf::Color::Green);
                sf::Event event;
                tmpstr += "Commands : +/-, up/down, [L]og, [O]bject, [G]rid, [Collision], space, esc \n";
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
                        else if (event.key.code == sf::Keyboard::O)
                            objectFlag = !objectFlag;
                        else if (event.key.code == sf::Keyboard::G)
                            gridFlag = !gridFlag;
                        else if (event.key.code == sf::Keyboard::C)
                            collisionFlag = !collisionFlag;
                        else if (event.key.code == sf::Keyboard::Numpad1)
                            initTest1();
                }
                window.clear();
            }
            updateDraw(timers);
            {
                ftw::timethat timet(timers, "windows.draw circles - BLUE", sf::Color::Blue);
                if (objectFlag) this->draw(window);
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
                std::vector<std::vector<size_t>> vCollisions;
                zone headZone { idEngine, currentPhysics, vCollisions };
                if (collisionFlag)
                    updateCollisions(vCollisions);
                std::vector<std::tuple<std::string, float, float>> strzlog; //s,x,y
                if (gridFlag)
                {
                    for (auto i = 0; i<vCollisions.size() ; i++)
                        for (auto c : vCollisions[i])
                        {
                            sf::Vertex line[2];
                            auto deltaRadius_i = sf::Vector2f(
                                currentPhysics[i].radius,
                                currentPhysics[i].radius);
                            auto deltaRadius_c = sf::Vector2f(
                                currentPhysics[c].radius,
                                currentPhysics[c].radius);
                            line[0].position = (currentPhysics[i].position + 
                                deltaRadius_i) * zoomVal + deltaPos;
                            line[1].position = (currentPhysics[c].position + 
                                deltaRadius_c) * zoomVal + deltaPos;
                            window.draw(line,2, sf::Lines);
                        }
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
                        e.setPosition(
                            e.getPosition().x * zoomVal + deltaPos.x, 
                            e.getPosition().y * zoomVal + deltaPos.y);
                        window.draw(e);
                    }
                    std::vector<sf::RectangleShape> zones;
                    headZone.to_rectangle(zones);
                    for (const auto& e : zones)
                    {
                        auto zoomedShape = e;
                        zoomedShape.setPosition(
                            e.getPosition().x * zoomVal + deltaPos.x,
                            e.getPosition().y * zoomVal + deltaPos.y);
                        zoomedShape.setSize(sf::Vector2f(
                            e.getSize().x * zoomVal,
                            e.getSize().y * zoomVal));
                        window.draw(zoomedShape);
                    }
                }
            }
            ftw::timethat timet(timers, "Windows.Display - MAGENTA", sf::Color::Magenta);
            window.display();
        }
    }
    void world_basedonvector::updatePhysics()
    {
        auto currentdt = std::chrono::system_clock::now();
        // linked to deletePhysics algorithm : should be done backward
        for (int i = (int)physics.size()-1; i >= 0; i--)
        {
            auto radius = currentPhysics[i].radius;
            auto mass = currentPhysics[i].mass;
            auto dtm = static_cast<float>(
                std::chrono::duration_cast<std::chrono::milliseconds> (
                    currentdt - physics[i].dt).count());
            float friction = currentPhysics[i].friction;
            auto force = currentPhysics[i].force;
            auto speed = currentPhysics[i].speed;
            auto position = physics[i].shape.getPosition();
            
            sf::Vector2f addedSpd;
            if (deltaSpeeds.size() > i)
            {
                float sz = (float)deltaSpeeds[i].size();
                for (auto& e : deltaSpeeds[i])
                    addedSpd += e;
                if (sz > 0)
                    addedSpd /= sz;
                //assert(!isnan(addedSpd.x || addedSpd.y));
                //assert(!isinf(addedSpd.x || addedSpd.y));
            }

            if (bool isReadyForEquaDiff = 
                speed.x != 0 || speed.y != 0 )
            {
                auto oldspd = speed;
                speed += addedSpd;
                position *= 1000.0f;
                auto ff = force / friction;
                float e_setdtm = exp(-friction / mass * dtm / 1000.0f);
                position = (ff * dtm + (ff - speed * 1000.0f) * mass / friction *
                    (e_setdtm - 1) + position * 1000.0f) / 1000.0f;
                speed = (ff + (speed * 1000.0f - ff) * e_setdtm) / 1000.0f;
                position /= 1000.0f;
                //assert(speed.x<10'000);
                assert(!isnan(speed.x)&& !isinf(speed.x));
                assert(!isnan(position.x));
            }
            deltaSpeeds[i].clear();
            if (bool isOutOfTerrain =
                position.x + radius<0 || position.x + radius>MAXTerrain ||
                position.y + radius<0 || position.y + radius>MAXTerrain)
                deletePhysics(i);
            else 
            {
                assert(!isnan(position.x));
                currentPhysics[i].position = position;
                assert(!isnan(speed.x));
                currentPhysics[i].speed = speed;
            }
        }
        for (auto i = 0; i < physics.size(); i++) //Debug
        {
            assert(i == currentPhysics[i].currentId);
            auto position = currentPhysics[i].position;
            auto radius = currentPhysics[i].radius;
            bool isOutOfTerrain =
                position.x + radius<0 || position.x + radius>MAXTerrain ||
                position.y + radius<0 || position.y + radius>MAXTerrain;
            assert(!isOutOfTerrain);
            assert(!isnan(position.x));
        }
    }
    //replace with the last element in the vector (witch should be hopefully already processed...)
    void inline world_basedonvector::deletePhysics(size_t iemElement)
    {
        if (currentPhysics.size() - 1 != iemElement)  //not the last one
        {
            if (iemElement == 44) std::cout << "DEL";
            currentPhysics[iemElement] = currentPhysics.back();
            currentPhysics[iemElement].currentId = iemElement;
            physics[iemElement] = physics.back();
        }
        deltaSpeeds.pop_back();
        currentPhysics.pop_back();
        physics.pop_back();
    }
}