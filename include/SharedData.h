#pragma once
#include "raylib.h"
#include <box2d/box2d.h>

struct SharedData {
    //constants
    const int WIDTH = 1200;
    const int HEIGHT = 900;
    b2World* world;
    // dynamic data
    Vector2 mousePos;
    Vector2 camTargetPos = {WIDTH / 2.0f, HEIGHT / 2.0f};
};
