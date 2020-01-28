#pragma once
#include <array>
#include <list>

const unsigned int MAXOBJ = 1024;
const unsigned int MAXPoint = 16;
const int MAXTerrain = 1024 * 2;
const int MINScreen_X = 0;
const int MINScreen_Y = 0;
const int MAXScreen_X = 1024;
const int MAXScreen_Y = 768;

namespace ftw
{
    template <class element>
    class container_basedonlist
    {
        std::list<element> dataToDraw;
        size_t nbelement {0};
    public:
        std::string type() { return "List"; }
        typename std::list<element>::iterator begin() { return dataToDraw.begin(); }
        typename std::list<element>::iterator end() { return dataToDraw.end(); };
        void add(const element& e) { dataToDraw.insert(dataToDraw.begin(), e); nbelement++; }
        size_t size() { return nbelement; }
    };

    template <class element>
    class container_basedonarray
    {
        std::array<element, MAXOBJ> datas;
        size_t currentpos{ 0 };
    public:
        std::string type() { return "Array"; }
        element* begin() { return (datas.size() == 0) ? nullptr : &datas.at(0); }
        element* end()
        {
            element* lastelem = &datas.at(datas.size() - 1);
            element* afterlastelem = ++lastelem;
            return afterlastelem;
        }
        void add(const element& e) { datas[currentpos++] = e; }
        size_t size() { return currentpos; }
    };

    template <class element>
    class container_basedonvector
    {
        std::vector<element> datas;
    public:
        std::string type() { return "Vector"; }
        element* begin() { return (datas.size() == 0) ? nullptr : &datas.at(0); }
        element* end()
        {
            element* lastelem = &datas.at(datas.size() - 1);
            element* afterlastelem = ++lastelem;
            return afterlastelem;
        }
        void add(const element& e) { datas.emplace_back(e); }
        size_t size() { return datas.size(); }
    };

    template <class element>
    using container = container_basedonvector<element>;
}