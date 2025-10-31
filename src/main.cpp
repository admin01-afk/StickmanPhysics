#include "raylib.h"
#include <box2d/box2d.h>
#include <iostream>
#include <vector>

#define DEBUG 1
#if DEBUG
    #define dPrint(fmt, ...) printf("[DEBUG] " fmt, ##__VA_ARGS__)
#else
    #define dPrint(...) ((void)0)
#endif

const int WIDTH = 800;
const int HEIGHT = 600;
const float SCALE = 50.0f;

b2Vec2 gravity(0.0f, -1.0f); // -10
b2World world(gravity);

#pragma region CoordinateConversions
Vector2 screen2world(Vector2 screenPos){
    return Vector2{screenPos.x / SCALE, -screenPos.y / SCALE};
}
//b2Vec2 to Vector2
Vector2 world2screen(b2Vec2 worldPos){
    return Vector2{worldPos.x * SCALE, -worldPos.y * SCALE};
}
#pragma endregion

class Renderer {
public:
    //meters to pixels
    float scale;
    Renderer(float scale) : scale(scale) {}

    void Draw(b2World* world) const {
        for (b2Body* body = world->GetBodyList(); body; body = body->GetNext()) {
            for (b2Fixture* fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
                const b2Shape* shape = fixture->GetShape();
                b2Shape::Type type = shape->GetType();
                b2Vec2 pos = body->GetPosition();
                float angle = body->GetAngle();

                Color bodyColor = ColorFromHSV((float)((uintptr_t)body % 360), 0.8f, 0.9f);
                

                switch (type) {
                    case b2Shape::e_circle: {
                        const b2CircleShape* circle = static_cast<const b2CircleShape*>(shape);
                        b2Vec2 center = body->GetWorldPoint(circle->m_p);
                        Vector2 screenCenter = world2screen(center);
                        float radius = circle->m_radius * scale;
                    #if DEBUG
                        DrawCircleLinesV(screenCenter, radius, bodyColor);
                    #else
                        DrawCircleV(screenCenter, radius, bodyColor);
                    #endif
                        break;
                    }

                    case b2Shape::e_polygon: {
                        const b2PolygonShape* poly = static_cast<const b2PolygonShape*>(shape);
                        int vertexCount = poly->m_count;

                        if (vertexCount >= 3) {
                            std::vector<Vector2> points(vertexCount);
                            for (int i = 0; i < vertexCount; ++i)
                                points[i] = world2screen(body->GetWorldPoint(poly->m_vertices[i]));
                        #if DEBUG
                            for (int i = 0; i < vertexCount; ++i)
                                DrawLineV(points[i], points[(i + 1) % vertexCount], bodyColor);
                        #else
                            for (int i = 1; i < vertexCount - 1; ++i)
                                DrawTriangle(points[0], points[i], points[i + 1], bodyColor);
                        #endif
                        }
                        break;
                    }

                    case b2Shape::e_edge: {
                        const b2EdgeShape* edge = static_cast<const b2EdgeShape*>(shape);
                        Vector2 p1 = world2screen(body->GetWorldPoint(edge->m_vertex1));
                        Vector2 p2 = world2screen(body->GetWorldPoint(edge->m_vertex2));
                        DrawLineV(p1, p2, BLUE);
                        break;
                    }

                    default: break;
                }
            }
        }
    }
};

void MousePosDebug(){
    #if !DEBUG
        return;
    #endif
    Vector2 mousePos = GetMousePosition();
    Vector2 worldPos = screen2world(mousePos);
    int fontSize = 20;

    // Calculate text content
    char screenText[64];
    char worldText[64];
    snprintf(screenText, sizeof(screenText), "screen: [%.0f, %.0f]", mousePos.x, mousePos.y);
    snprintf(worldText, sizeof(worldText), "world: [%.2f, %.2f]", worldPos.x, worldPos.y);

    // Calculate width
    float textWidth = MeasureText(screenText, fontSize);

    // Determine final draw position (with padding)
    float padding = 10;
    float x = mousePos.x + padding;
    float y = mousePos.y;

    // If text would overflow to the right, move it left
    if (x + textWidth + padding > GetScreenWidth()) {
        x = mousePos.x - textWidth - padding;
    }

    // If text would overflow off bottom edge, move it up
    if (y + fontSize * 2 + padding > GetScreenHeight()) {
        y = GetScreenHeight() - fontSize * 2 - padding;
    }

    // If text would go above top edge, push it down
    if (y < padding) {
        y = padding;
    }

    // Draw both lines
    DrawText(screenText, x, y, fontSize, ORANGE);
    DrawText(worldText, x, y + fontSize, fontSize, RED);
}

int main() {
    InitWindow(WIDTH, HEIGHT, "Stickman Ragdoll");
    SetTargetFPS(60);
    Renderer renderer(SCALE);

    b2BodyDef grounddef;
    grounddef.type = b2_staticBody;
    grounddef.position.Set(WIDTH / SCALE / 2.0f, -(HEIGHT / SCALE) + 0.5f);
    dPrint("Ground position: (%.2f, %.2f)\n", grounddef.position.x, grounddef.position.y);
    b2Body* ground = world.CreateBody(&grounddef);
    b2PolygonShape boxshape;
    boxshape.SetAsBox(8.0f, 0.5f);
    ground->CreateFixture(&boxshape, 1.0f);

    while (!WindowShouldClose()) {
        world.Step(1.0f / 60.0f, 8, 3);
        BeginDrawing();
            ClearBackground(BLACK);
            #if DEBUG
            for (size_t i = 0; i < WIDTH/SCALE; i++) //grid
            {
                DrawLine(0, i * SCALE, WIDTH, i * SCALE, (i==0) ? RED : GRAY);
                DrawLine((i * SCALE)+1, 0, i * SCALE, HEIGHT, (i==0) ? BLUE : GRAY);
            }
            #endif
            renderer.Draw(&world);
            MousePosDebug();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}