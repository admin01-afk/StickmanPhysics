#include "physics.h"
#include "input.h"
#include "render.h"
#include "SharedData.h"
#include "box2d/box2d.h"

int main() {
    SharedData sharedData;
    Physics physics(&sharedData);
    InputHandler inputHandler(&sharedData);
    Renderer renderer(sharedData.world, &sharedData);

    physics.createGround();
    physics.CreateStickman({10.0f, 5.0f});

    while (!WindowShouldClose()) {
        inputHandler.ProcessInput();
        physics.updatePhysics();
        renderer.Render();
    }
    CloseWindow();
    return 0;
}
