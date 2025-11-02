#pragma once
#include "raylib.h"
#include <box2d/box2d.h>

struct SharedData {
    //constants
    b2World* world;
    // dynamic data
    Vector2 mousePos;
};
