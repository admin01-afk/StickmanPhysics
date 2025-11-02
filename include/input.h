#pragma once
#include "EventBus.h"
#include "Events.h"
#include "SharedData.h"

#include "raylib.h"

class InputHandler {
public:
    SharedData* sharedData;
    InputHandler(SharedData* data) : sharedData(data) {}
    void ProcessInput();
private:

    float zoomSpeed = 1.0f;
    bool isPhysicsPaused = false;
};