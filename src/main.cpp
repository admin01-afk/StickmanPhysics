#include "physics.h"
#include "input.h"
#include "render.h"
#include "SharedData.h"

int main() {
    SharedData sharedData;
    Physics physics(&sharedData);
    InputHandler inputHandler(&sharedData);
    Renderer renderer(sharedData.world, &sharedData);

    physics.createGround(sharedData.WIDTH / (2.0f * renderer.SCALE), -sharedData.HEIGHT / renderer.SCALE);
    physics.CreateStickman({sharedData.WIDTH / (2.0f * renderer.SCALE), -sharedData.HEIGHT / renderer.SCALE + 5.0f});

    while (!WindowShouldClose()) {
        inputHandler.ProcessInput();
        physics.updatePhysics();
        renderer.UpdateCameraTargetPos();
        renderer.Render();
    }
    CloseWindow();
    return 0;
}