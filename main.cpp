//https://github.com/keldenworld/circles
#include <iostream>
#include <ctime>
#include "world.h"

int main()
{
    auto world = ftw::world();
    world.box2DGround();
    world.box2DSquare();
    world.loop();
}
