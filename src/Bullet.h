#ifndef BULLET_H
#define BULLET_H

#pragma once
#include <vector>
#include "raylib.h"

class Bullet {
public:
    Rectangle rect;
    bool active;

    Bullet(); // sigue estando
    Bullet(Rectangle r, bool a) : rect(r), active(a) {}
};

#endif