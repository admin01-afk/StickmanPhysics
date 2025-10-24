#include "raylib.h"
#include <box2d/box2d.h>
#include "stickman.h"

#include <iostream>

const int WIDTH = 800;
const int HEIGHT = 600;
const float SCALE = 50.0f;

b2Vec2 gravity(0.0f, 0.0f); // -10
b2World world(gravity);

//ground
b2Body* ground;
void Draw(b2World* world, float scale);

int main() {
    // Ground Box
    b2BodyDef groundDef;
    Vector2 gpos = {0.0f, 1.0f};
    groundDef.position.Set(gpos.x,gpos.y);
    ground = world.CreateBody(&groundDef);
    b2PolygonShape groundBox;
    float gwidth = (WIDTH / SCALE / 2); // 16 meters wide
    float gheight = 0.5f; // 1 meter thick
    groundBox.SetAsBox(gwidth, gheight);
    ground->CreateFixture(&groundBox, 0.0f);

    InitWindow(WIDTH, HEIGHT, "Stickman Ragdoll");
    SetTargetFPS(60);
    // box2d (meters) -> raylib (pixels)
    
    
    Stickman stickman(&world, 0, HEIGHT / SCALE / 2);

    while (!WindowShouldClose()) {
        world.Step(1.0f / 60.0f, 8, 3);

        BeginDrawing();
        ClearBackground(BLACK);
        Draw(&world, SCALE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

void Draw(b2World* world, float scale) {
    for (b2Body* body = world->GetBodyList(); body; body = body->GetNext()) {
        for (b2Fixture* fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
            const b2Shape* shape = fixture->GetShape();
            b2Shape::Type type = shape->GetType();
            b2Vec2 pos = body->GetPosition();
            float angle = body->GetAngle();

            switch (type) {
                case b2Shape::e_circle: {
                    const b2CircleShape* circle = static_cast<const b2CircleShape*>(shape);
                    b2Vec2 center = body->GetWorldPoint(circle->m_p);
                    Vector2 screenCenter = {
                        center.x * scale + WIDTH / 2.0f,
                        HEIGHT - center.y * scale
                    };
                    float radius = circle->m_radius * scale;
                    DrawCircleLinesV(screenCenter, radius, GREEN);
                    break;
                }

                case b2Shape::e_polygon: {
                    const b2PolygonShape* poly = static_cast<const b2PolygonShape*>(shape);
                    int vertexCount = poly->m_count;

                    for (int i = 0; i < vertexCount; ++i) {
                        b2Vec2 v1 = body->GetWorldPoint(poly->m_vertices[i]);
                        b2Vec2 v2 = body->GetWorldPoint(poly->m_vertices[(i + 1) % vertexCount]);

                        Vector2 p1 = { v1.x * scale + WIDTH / 2.0f, HEIGHT - v1.y * scale };
                        Vector2 p2 = { v2.x * scale + WIDTH / 2.0f, HEIGHT - v2.y * scale };

                        DrawLineV(p1, p2, YELLOW);
                    }
                    break;
                }

                case b2Shape::e_edge: {
                    const b2EdgeShape* edge = static_cast<const b2EdgeShape*>(shape);
                    b2Vec2 v1 = body->GetWorldPoint(edge->m_vertex1);
                    b2Vec2 v2 = body->GetWorldPoint(edge->m_vertex2);

                    Vector2 p1 = { v1.x * scale + WIDTH / 2.0f, HEIGHT - v1.y * scale };
                    Vector2 p2 = { v2.x * scale + WIDTH / 2.0f, HEIGHT - v2.y * scale };

                    DrawLineV(p1, p2, BLUE);
                    break;
                }

                default:
                    break;
            }
        }
    }
}