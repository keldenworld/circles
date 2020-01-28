#include "world.h"

namespace ftw
{
    void world_basedonvector::initload()
    {
        sf::CircleShape shape(100);
        shape.setPointCount(MAXPoint);
        shape.setFillColor(sf::Color::Transparent);
        shape.setOutlineThickness(1);
        for (int i = 0; i < MAXOBJ; i++)
        {
            shape.setPosition(static_cast<float>(rand() % MAXTerrain - 255) + 255, static_cast<float>(rand() % MAXTerrain - 255) + 255);
            shape.setRadius(static_cast<float>(rand() % (64 + 3) - 3));
            shape.setOutlineColor(sf::Color(rand() % 255, rand() % 255, rand() % 255));
            currentPhysics.emplace_back(sf::Vector2f(shape.getPosition().x * zoom, shape.getPosition().y * zoom));
            physics.emplace_back(data(shape,
                sf::Vector2f(static_cast<float>(rand() % 32) - 16.0f, static_cast<float>(rand() % 32) - 16.0f),
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
            "Zoom : " + std::to_string(zoom) + "\n" +
            "Drawned Elements : " + std::to_string(drawingPhysics.size()) + "\n" +
            "Density : " + std::to_string(density) + "\n";
    }
    void world_basedonvector::setDeltaPos(int x, int y)
    {
        deltaPos.x += x;
        deltaPos.y += y;
        if (deltaPos.x > 0) deltaPos.x = 0;
        if (deltaPos.y > 0) deltaPos.y = 0;
        if (deltaPos.x < -MAXTerrain + MAXScreen_X) deltaPos.x = -MAXTerrain + MAXScreen_X;
        if (deltaPos.y < -MAXTerrain + MAXScreen_Y) deltaPos.y = -MAXTerrain + MAXScreen_Y;
    }

    void world_basedonvector::draw(sf::RenderWindow& window)
    {
        for (auto e : drawingPhysics) window.draw(e);
    }

    void world_basedonvector::updateDraw(std::map<std::string, std::tuple<sf::Color, float>>& timers)
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
    void world_basedonvector::zoomUpdate()
    {
        if (zoom < 0.2f) zoom = 0.2f;
        for (auto& e : this->physics)
            e.zoomedShape.setRadius(e.shape.getRadius() * zoom);
    }
    void world_basedonvector::loop()
    {
        sf::RenderWindow window(sf::VideoMode(MAXScreen_X, MAXScreen_Y), "Hello Kelden World with SFML!");
        window.setVerticalSyncEnabled(false);

        sf::Clock clock;
        std::string tmpstr;
        sf::Text text2prt;
        text2prt.setCharacterSize(21);
        text2prt.setFillColor(sf::Color::White);
        sf::Font font;
        if (!font.loadFromFile("monofonto.ttf")) return;
        text2prt.setFont(font);

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
                tmpstr += "Commands : +/-, up/down, l, space, esc \n";
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
                for (auto r : timet.to_rectangle()) window.draw(r);
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
                    tmpstr += "\nFPS (basics World Vector) : " + 
                        std::to_string(myfps.get()) + "\n";
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

            if (bool isOutOfTerrain = newx<0 || newx>MAXTerrain || newy<0 || newy>MAXTerrain)
                deletePhysics(i);
            else
                currentPhysics[i] = sf::Vector2f(newx * zoom, newy * zoom);
        }
    }
    //replace with the last element in the vector (witch should be hopefully already processed...)
    void world_basedonvector::deletePhysics(size_t iemElement)
    {
        if (currentPhysics.size() - 1 != iemElement)  //not the last one
        {
            currentPhysics[iemElement] = currentPhysics.back();
            drawingPhysics[iemElement] = drawingPhysics.back();
            physics[iemElement] = physics.back();
        }
        currentPhysics.pop_back();
        drawingPhysics.pop_back();
        physics.pop_back();
    }
}