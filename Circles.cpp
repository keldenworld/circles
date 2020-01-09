//https://github.com/keldenworld/circles
#include <iostream>
#include <ctime>
#include <SFML/Graphics.hpp>

namespace ftw
{
    template <class T>
    class container_basedonvector 
    {
        std::vector<T> datas;
    public:
        T* begin() { return (datas.size() == 0) ? nullptr : &datas.at(0); }
        T* end() 
        { 
            T* lastelem = &datas.at(datas.size() - 1);
            T* afterlastelem = ++lastelem;
            return afterlastelem;
        }

        void add(const T& e) { datas.emplace_back(e); }
        size_t size() { return datas.size(); }
    };

    template <class T>
    using container = container_basedonvector<T>;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(1024, 1024), "Hello Kelden World with SFML!");
    auto circles = ftw::container<sf::CircleShape>();

    sf::CircleShape shape(100);
    for (int i = 0; i < 1024; i++)
    {
        shape.setPosition(static_cast<float>(rand() % 1024), static_cast<float>(rand() % 1024));
        shape.setRadius(static_cast<float>(rand() % (255 - 5) + 5));
        shape.setFillColor(sf::Color(rand() % 255, rand() % 255, rand() % 255));
        circles.add(shape);
    }

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
            if (event.type == sf::Event::Closed)
                window.close();

        window.clear();

        for (auto &s : circles)
            window.draw(s);

        window.display();
    }
    return 0;
}