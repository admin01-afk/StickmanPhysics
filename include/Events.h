#pragma once
#include "raylib.h"

struct ToggleDebugEvent {};

struct setCameraTargetEvent {
    Vector2 target;
};

struct CameraZoomEvent {
    float zoom;
};

struct PausePhysicsEvent
{
    bool isPaused;
};

struct OnMouseDownEvent{};
struct OnMouseMoveEvent{};
struct OnMouseUpEvent{};
