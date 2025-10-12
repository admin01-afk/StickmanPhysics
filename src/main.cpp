#include "raylib.h"
#include <box2d/box2d.h>
#include "stickman.h"

#include <iostream>

const int WIDTH = 800;
const int HEIGHT = 600;
const float SCALE = 50.0f;

// 0,0 -> WIDTH/2, HEIGHT
Vector2 b2Pos2screen(b2Vec2 pos) {
    // Box2D (0,0) is world origin; we want that at screen bottom center
    return {
        pos.x * SCALE + WIDTH / 2.0f,  // shift x to center
        HEIGHT - pos.y * SCALE         // flip y and shift origin to bottom
    };
}

Rectangle btn = {10, 10, 160, 40};
bool clicked = false;

// create the world
b2Vec2 gravity(0.0f, 0.0f);
b2World world(gravity);

void ToggleGravity() {
    b2Vec2 newGravity = {0, (world.GetGravity().y != 0) ? 0 : -10.0f};
    world.SetGravity(newGravity);
    std::cout << "Gravity: " << world.GetGravity().y << "\n";

    // Wake up all bodies so they respond to new gravity
    if(world.GetGravity().y != 0)
    for (b2Body* body = world.GetBodyList(); body; body = body->GetNext()) {
        if (body->GetType() == b2_dynamicBody) {
            body->SetAwake(true);
        }
    }
}
int main() {
    InitWindow(WIDTH, HEIGHT, "Stickman Ragdoll");
    SetTargetFPS(60);
    // box2d (meters) -> raylib (pixels)
    
    
    // Ground Box
    b2BodyDef groundDef;
    groundDef.position.Set(0.0f,0.0f);
    b2Body* ground = world.CreateBody(&groundDef);
    b2PolygonShape groundBox;
    groundBox.SetAsBox(10.0f, 0.5f);
    ground->CreateFixture(&groundBox, 0.0f);
    
    Stickman stickman(&world, 0, 10, SCALE);

    while (!WindowShouldClose()) {
        world.Step(1.0f / 60.0f, 8, 3);

        Vector2 mouse = GetMousePosition();
        bool hovered = CheckCollisionPointRec(mouse, btn);
        bool pressed = hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

        if (pressed) clicked = !clicked;

        BeginDrawing();
        ClearBackground(BLACK);
        DrawRectangleRec(btn, hovered ? LIGHTGRAY : GRAY);
        DrawText("toggle gravity", btn.x + 10, btn.y + 10, 20, BLACK);
        if (pressed) {
            clicked = !clicked;
            ToggleGravity();  // call once per click
        }

        stickman.Draw(SCALE);

        // Draw ground
        b2Vec2 gpos = ground->GetPosition();
        Vector2 screenPos = b2Pos2screen(gpos);
        float gwidth = 10.0f * SCALE;
        float gheight = 0.5f * SCALE;
        DrawRectanglePro(
            { screenPos.x, screenPos.y, gwidth * 2, gheight * 2 },
            { gwidth, gheight },
            0,
            WHITE
        );
        EndDrawing();
    }

    CloseWindow();
    return 0;
}