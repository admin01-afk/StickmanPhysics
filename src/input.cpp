#include "input.h"
#include "EventBus.h"
#include "Events.h"
#include "raylib.h"

/*
    input processing logic
*/ 
void InputHandler::ProcessInput() {
    float dt = GetFrameTime();

    // Update mouse position
    sharedData->mousePos = GetMousePosition();

    // Toggle debug mode
    if (IsKeyPressed(KEY_F3)) EventBus::Emit(ToggleDebugEvent{});
    
    // Move camera target
    if (IsKeyDown(KEY_D)) sharedData->camTargetPos.x += 200 * dt;
    if (IsKeyDown(KEY_A)) sharedData->camTargetPos.x -= 200 * dt;
    if (IsKeyDown(KEY_S)) sharedData->camTargetPos.y += 200 * dt;
    if (IsKeyDown(KEY_W)) sharedData->camTargetPos.y -= 200 * dt;

    //zoom
    float zoom = 0.0f;
    if (IsKeyDown(KEY_Q)) zoom -= zoomSpeed * dt;
    if (IsKeyDown(KEY_E)) zoom += zoomSpeed * dt;
    if (zoom != 0.0f) EventBus::Emit(CameraZoomEvent{zoom});

    // Handle mouse button events
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) EventBus::Emit(OnMouseDownEvent{});
    else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) EventBus::Emit(OnMouseUpEvent{});
    else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) EventBus::Emit(OnMouseMoveEvent{});

    // toggle physics
    if (IsKeyPressed(KEY_P)) {
        isPhysicsPaused = !isPhysicsPaused;
        EventBus::Emit(PausePhysicsEvent{isPhysicsPaused});
    }
}
