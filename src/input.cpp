#include "input.h"

// input processing logic
void InputHandler::ProcessInput() {
    float dt = GetFrameTime();

    // Update mouse position
    sharedData->mousePos = GetMousePosition();

    // Toggle debug mode
    if (IsKeyPressed(KEY_F3)) EventBus::Emit(ToggleDebugEvent{});
    
    // Move camera target
    Vector2 target = {0, 0};
    if (IsKeyDown(KEY_D)) target.x += 200 * dt;
    if (IsKeyDown(KEY_A)) target.x -= 200 * dt;
    if (IsKeyDown(KEY_S)) target.y += 200 * dt;
    if (IsKeyDown(KEY_W)) target.y -= 200 * dt;
    if (target.x != 0 || target.y != 0) EventBus::Emit(setCameraTargetEvent{target});

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
