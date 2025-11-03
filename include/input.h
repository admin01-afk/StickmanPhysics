#pragma once
#include "SharedData.h"


class InputHandler {
public:
    SharedData* sharedData;
    InputHandler(SharedData* data) : sharedData(data) {}
    void ProcessInput();
private:
    float zoomSpeed = 1.0f;
    bool isPhysicsPaused = false;
};