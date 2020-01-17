#pragma once
#include <array>
#include <list>

const int MAXOBJ = 1024;
const int MAXPoint = 16;

namespace ftw
{
    template <class element>
    class container_basedonlist
    {
        std::list<element> head;
        size_t nbelement;
    public:
        std::string type() { return "List"; }
        typename std::list<element>::iterator begin() { return head.begin(); }
        typename std::list<element>::iterator end() { return head.end(); };
        element& operator ++ () { return std::next(element, 1); }
        container_basedonlist() : nbelement{ 0 } {}
        void add(const element& e) { head.insert(head.begin(), e); nbelement++; }
        size_t size() { return nbelement; }
    };

    template <class element>
    class container_basedonarray
    {
        std::array<element, MAXOBJ> datas;
        size_t currentpos;
    public:
        std::string type() { return "Array"; }
        element* begin() { return (datas.size() == 0) ? nullptr : &datas.at(0); }
        element* end()
        {
            element* lastelem = &datas.at(datas.size() - 1);
            element* afterlastelem = ++lastelem;
            return afterlastelem;
        }
        container_basedonarray() : currentpos{ 0 } {}
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