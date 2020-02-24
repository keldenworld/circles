#include "world.h"
#include "zone.h"
#include "assert.h"

namespace ftw
{
    void world::box2DGround()
    {
        b2BodyDef groundBodyDef;
        groundBodyDef.position.Set(0.0f, -10.0f);
        b2Body* groundBody = b2World.CreateBody(&groundBodyDef);
        b2PolygonShape groundBox;
        groundBox.SetAsBox(50.0f, 10.0f);
        groundBody->CreateFixture(&groundBox, 0.0f);
    }
    void world::box2DPyramid()
    {
        float a = 0.5f;
        b2PolygonShape shape;
        shape.SetAsBox(a, a);

        b2Vec2 x(-7.0f, 0.75f);
        b2Vec2 y;
        b2Vec2 deltaX(0.5625f, 1.25f);
        b2Vec2 deltaY(1.125f, 0.0f);

        for (int32 i = 0; i < 20; ++i)
        {
            y = x;

            for (int32 j = i; j < 20; ++j)
            {
                b2BodyDef bd;
                bd.type = b2_dynamicBody;
                bd.position = y;
                b2Body* body = b2World.CreateBody(&bd);
                body->CreateFixture(&shape, 5.0f);

                y += deltaY;
            }

            x += deltaX;
        }
    }

    void world::box2DSquare()
    {
        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position.Set(0.0f, 4.0f);
        vBodies.emplace_back(b2World.CreateBody(&bodyDef));
        b2PolygonShape dynamicBox;
        dynamicBox.SetAsBox(1.0f, 1.0f);
        b2FixtureDef fixtureDef;
        fixtureDef.shape = &dynamicBox;
        fixtureDef.density = 1.0f;
        fixtureDef.friction = 0.3f;
        vBodies.back()->CreateFixture(&fixtureDef);
        float timeStep = 1.0f / 60.0f;
        int32 velocityIterations = 6;
        int32 positionIterations = 2;
        for (int32 i = 0; i < 60; ++i)
        {
            b2World.Step(timeStep, velocityIterations, positionIterations);
            for (auto &e : vBodies)
            {
                b2Vec2 position = e->GetPosition();
                float angle = e->GetAngle();
                std::cout << position.x << " / " << position.y << " / " << angle;
            }
        }
    }
    void world::log(std::string& txt)
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
    void world::setDeltaPos(int x, int y)
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

    void world::draw(sf::RenderWindow& window) 
    {
        for (const auto e : drawingPhysics)
            window.draw(e);
    }

    void world::updateDraw(std::map<std::string, std::tuple<std::string, sf::Color, float>>& timers)
    {
        sf::Vector2f v2f;
        sf::CircleShape shape;
        size_t n = 0;
        for (auto i = 0; i < physics.size(); i++)
        {
            {
                ftw::timethat timet(timers, "update draw [compute position]");
                v2f = this->currentPhysics[i].position;
                v2f *= zoomVal;
                v2f += deltaPos;
            }
            if (bool isVisible = (v2f.x < MAXScreen_X&& v2f.y < MAXScreen_Y))
            {
                ftw::timethat timet(timers, "update draw [insert into drawingPhysics]");
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
            ftw::timethat timet(timers, "update draw [clean drawing vector]");
            if (bool containOldDataAtTheEndOfDrawingVector = (n != drawingPhysics.size()))
                drawingPhysics.erase(drawingPhysics.begin() + n, drawingPhysics.end());
        }
    }
    void world::zoomUpdate()
    {
        if (zoomVal < 0.2f) zoomVal = 0.2f;
        for (auto i = 0 ; i< physics.size() ; i++)
            physics[i].shape.setRadius(currentPhysics[i].radius * zoomVal);
    }
    void world::initload()
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
    void world::setCircle(sf::CircleShape& shape, sf::Vector2f position, float radius, sf::Vector2f speed, sf::Vector2f force, float mass, sf::Color col) {
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
    }
    void world::initTest1()
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
    void world::loop()
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

        std::map<std::string, std::tuple<std::string, sf::Color, float>> timers;
        auto myfps = ftw::FPS();
        while (window.isOpen())
        {
            tmpstr = "";
            {
                ftw::timethat timet(timers, "update physics [equadiff]");
                this->updatePhysics();
            }
            {
                ftw::timethat timet(timers, "event loop + window.clear");
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
                            {zoomVal -= 0.2f; zoomUpdate();}
                        else if (event.key.code == sf::Keyboard::Add)
                            {zoomVal += 0.2f; zoomUpdate();}
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
                ftw::timethat timet(timers, "windows.draw circles");
                if (objectFlag) this->draw(window);
            }
            {
                ftw::timethat timet(timers, "log Rectangle");
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
                ftw::timethat timet(timers, "log text");
                log(tmpstr);
                tmpstr += timet.to_string();
            }
            if (logFlag)
            {
                {
                    ftw::timethat timet(timers, "log FPS");
                    myfps.update();
                    auto fpsval = myfps.get();
                    tmpstr += "\nFPS (basics World Vector) : " + 
                        std::to_string(fpsval) + "\n";
                    text2prt.setFillColor((fpsval > 60 ? sf::Color::Green : sf::Color::White));
                }
                {
                    ftw::timethat timet(timers, "prepare text log");
                    text2prt.setString("\n\n" + tmpstr);
                }
                {
                    ftw::timethat timet(timers, "print text log");
                    window.draw(text2prt);
                }
            }
            {
                ftw::timethat timet(timers, "Generate Zones");
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
            ftw::timethat timet(timers, "Windows.Display");
            window.display();
        }
    }
    void world::updatePhysics()
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
    void world::deletePhysics(size_t iemElement)
    {
        if (currentPhysics.size() - 1 != iemElement)  //not the last one
        {
            //if (iemElement == 44) std::cout << "DEL";
            currentPhysics[iemElement] = currentPhysics.back();
            currentPhysics[iemElement].currentId = iemElement;
            physics[iemElement] = physics.back();
        }
        deltaSpeeds.pop_back();
        currentPhysics.pop_back();
        physics.pop_back();
    }
    inline void world::updateCollisions(std::vector<std::vector<size_t>>& vCollisions)
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
            int i = 0;
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
                    std::cout << c1.speed.x << "(" << c1.radius << ")[" << i << "/" << e << "], ";
                //assert(c2.speed.x < 10'000);
                float energyExit = c1.mass *
                    sqrt(c1.speed.x * c1.speed.x + c1.speed.y * c1.speed.y) +
                    c2.mass *
                    sqrt(c2.speed.x * c2.speed.x + c2.speed.y * c2.speed.y);
                if (energyExit <= 0.0f) energyExit = 1.0f;
                if (energyExit > 2.0f) energyExit = 2.0f;
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
}