#pragma once
#include "SharedData.h"

#include "raylib.h"
#include <box2d/box2d.h>


/*  b2World *world  */
class Renderer {
public:
    bool DEBUG = true;
    const float SCALE = 50.0f; // meters to pixels
    Camera2D camera;
    b2MouseJoint *mouseJoint = nullptr;

    Renderer(b2World *world, SharedData* sharedData);
    void UpdateCameraTargetPos();
    void Render();
private:
    b2World *world;
    SharedData* sharedData;
    // functions
    void Draw(b2World *world);
    void MousePosDebug();
    void DrawGrid();
    
    b2Vec2 mouseWorld;
    void OnMouseDown(); // Why are these here? sue me :)
    void OnMouseMove(); // TODO: move mouse grabbing logic somewhere else
    void OnMouseUp();

    #pragma region CoordinateConversions
    Vector2 screen2world(Vector2 screenPos)
    {
        Vector2 world = GetScreenToWorld2D(screenPos, camera);
        return {world.x / SCALE, -world.y / SCALE};
    }

    b2Vec2 screen2worldb2(Vector2 screenPos)
    {
        Vector2 world = GetScreenToWorld2D(screenPos, camera);
        return b2Vec2(world.x / SCALE, -world.y / SCALE);
    }
    // b2Vec2 to Vector2
    Vector2 world2screen(b2Vec2 worldPos)
    {
        return Vector2{worldPos.x * SCALE, -worldPos.y * SCALE};
    }
    #pragma endregion
};