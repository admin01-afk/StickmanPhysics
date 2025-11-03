#include "render.h"
#include <cstdio>
#include "EventBus.h"
#include "Events.h"
#include <vector>

Renderer::Renderer(b2World *world, SharedData* sharedData) : world(world), sharedData(sharedData) {
    InitWindow(sharedData->WIDTH, sharedData->HEIGHT, "Window");
    SetTargetFPS(60);
    camera = {0};
    camera.target = {sharedData->WIDTH / 2.0f, sharedData->HEIGHT / 2.0f};
    camera.offset = {sharedData->WIDTH / 2.0f, sharedData->HEIGHT / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    EventBus::Subscribe<ToggleDebugEvent>([this](const ToggleDebugEvent&) { DEBUG = !DEBUG; });

    EventBus::Subscribe<setCameraTargetEvent>([this](const setCameraTargetEvent& e) {
        camera.target = e.target;
        std::printf("camera target update");
    });

    EventBus::Subscribe<CameraZoomEvent>([this](const CameraZoomEvent& e) {
        camera.zoom += e.zoom * camera.zoom; // zoom relative to current zoom, idk otherwise it's weird
        if (camera.zoom > 5.0f)
            camera.zoom = 5.0f; // max zoom in
        if (camera.zoom < 0.1f)
            camera.zoom = 0.1f; // max zoom out
    });

    EventBus::Subscribe<OnMouseDownEvent>([this](const OnMouseDownEvent&) { OnMouseDown(); });
    EventBus::Subscribe<OnMouseMoveEvent>([this](const OnMouseMoveEvent& ) { OnMouseMove(); });
    EventBus::Subscribe<OnMouseUpEvent>([this](const OnMouseUpEvent&) { OnMouseUp(); });
}

void Renderer::Render()
{
    BeginDrawing();
    BeginMode2D(camera); // draw ui realative to camera
    ClearBackground(BLACK);
    DrawGrid();
    Draw(world);
    EndMode2D();
    MousePosDebug();
    EndDrawing();
}

void Renderer::Draw(b2World *world)
{
    for (b2Body *body = world->GetBodyList(); body; body = body->GetNext())
    {
        for (b2Fixture *fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext())
        {
            const b2Shape *shape = fixture->GetShape();
            b2Shape::Type type = shape->GetType();
            b2Vec2 pos = body->GetPosition();
            float angle = body->GetAngle();

            Color bodyColor = ColorFromHSV((float)((uintptr_t)body % 360), 0.8f, 0.9f);

            switch (type)
            {
            case b2Shape::e_circle:
            {
                const b2CircleShape *circle = static_cast<const b2CircleShape *>(shape);
                b2Vec2 center = body->GetWorldPoint(circle->m_p);
                Vector2 screenCenter = world2screen(center);
                float radius = circle->m_radius * SCALE;
                if (DEBUG) {
                    DrawCircleLinesV(screenCenter, radius, bodyColor);
                } else {
                    DrawCircleV(screenCenter, radius, bodyColor);
                }
                break;
            }

            case b2Shape::e_polygon:
            {
                const b2PolygonShape *poly = static_cast<const b2PolygonShape *>(shape);
                int vertexCount = poly->m_count;

                if (vertexCount >= 3)
                {
                    std::vector<Vector2> points(vertexCount);
                    for (int i = 0; i < vertexCount; ++i)
                        points[i] = world2screen(body->GetWorldPoint(poly->m_vertices[i]));
                    if (DEBUG) {
                        for (int i = 0; i < vertexCount; ++i)
                            DrawLineV(points[i], points[(i + 1) % vertexCount], bodyColor);
                    } else {

                        for (int i = 1; i < vertexCount - 1; ++i)
                        DrawTriangle(points[0], points[i], points[i + 1], bodyColor);
                    }
                }
                break;
            }

            case b2Shape::e_edge:
            {
                const b2EdgeShape *edge = static_cast<const b2EdgeShape *>(shape);
                Vector2 p1 = world2screen(body->GetWorldPoint(edge->m_vertex1));
                Vector2 p2 = world2screen(body->GetWorldPoint(edge->m_vertex2));
                DrawLineV(p1, p2, BLUE);
                break;
            }

            default:
                break;
            }

            if (mouseJoint)
            {
                b2Vec2 p1 = mouseJoint->GetAnchorA();
                b2Vec2 p2 = mouseJoint->GetAnchorB();
                DrawLineV(world2screen(p1), world2screen(p2), RED);
            }
        }
    }
}

void Renderer::UpdateCameraTargetPos()
{
    camera.target = sharedData->camTargetPos;
}

void Renderer::MousePosDebug()
{
    if(!DEBUG) return;

    // Get current mouse in both screen and world coords
    Vector2 mousePos = GetMousePosition();
    Vector2 worldPos = screen2world(mousePos);

    const int fontSize = 18;
    const float padding = 10.0f;

    // Prepare text
    char screenText[64];
    char worldText[64];
    snprintf(screenText, sizeof(screenText), "Screen: [%.0f, %.0f]", mousePos.x, mousePos.y);
    snprintf(worldText, sizeof(worldText), "World:  [%.2f, %.2f]", worldPos.x, worldPos.y);

    // Compute text sizes
    float screenWidth = MeasureText(screenText, fontSize);
    float worldWidth = MeasureText(worldText, fontSize);
    float textWidth = fmaxf(screenWidth, worldWidth);
    float textHeight = fontSize * 2;

    // Default position (to the right of the mouse)
    float x = mousePos.x + padding;
    float y = mousePos.y;

    // Prevent overflow to the right
    if (x + textWidth + padding > GetScreenWidth())
        x = mousePos.x - textWidth - padding;

    // Prevent overflow to bottom/top
    if (y + textHeight + padding > GetScreenHeight())
        y = GetScreenHeight() - textHeight - padding;
    else if (y < padding)
        y = padding;

    // Draw a background for readability
    DrawRectangleRounded((Rectangle){x - padding / 2, y - padding / 2, textWidth + padding, textHeight + padding / 2}, 0.2f, 4, Fade(BLACK, 0.4f));

    // Draw both lines
    DrawText(screenText, x, y, fontSize, ORANGE);
    DrawText(worldText, x, y + fontSize, fontSize, RED);
}

void Renderer::DrawGrid(){
    if (DEBUG){
        for (int i = 0; i <= sharedData->WIDTH / SCALE; i++)
        {
            Color c = (i == 0) ? RED : DARKGRAY;
            DrawLineV({i * SCALE + 1.0f, 0.0f}, {i * SCALE, (float)sharedData->HEIGHT}, c);
        }
        for (int j = 0; j <= sharedData->HEIGHT / SCALE; j++)
        {
            Color c = (j == 0) ? BLUE : DARKGRAY;
            DrawLineV({0.0f, j * SCALE}, {(float)sharedData->WIDTH, j * SCALE}, c);
        }
    }
}

void Renderer::OnMouseDown()
{
    mouseWorld = screen2worldb2(sharedData->mousePos);

    // AABB query around mouse to find body
    b2AABB aabb;
    b2Vec2 d(0.001f, 0.001f);
    aabb.lowerBound = mouseWorld - d;
    aabb.upperBound = mouseWorld + d;

    struct QueryCallback : b2QueryCallback
    {
        b2Vec2 point;
        b2Fixture *fixture = nullptr;
        bool ReportFixture(b2Fixture *f) override
        {
            if (f->GetBody()->GetType() == b2_dynamicBody && f->TestPoint(point))
            {
                fixture = f;
                return false; // stop
            }
            return true;
        }
    } callback;
    callback.point = mouseWorld;
    world->QueryAABB(&callback, aabb);

    if (callback.fixture)
    {
        b2Body *body = callback.fixture->GetBody();
        b2MouseJointDef mjd;
        b2BodyDef tempDef; // temporary static body creation for mouse joint, inefficient but whatever
        b2Body* tempStatic = world->CreateBody(&tempDef);
        mjd.bodyA = tempStatic;
        mjd.bodyB = body;
        mjd.target = mouseWorld;
        mjd.maxForce = 1000.0f * body->GetMass();
        mjd.stiffness = 50.0f;
        mjd.damping = 5.0f;
        mouseJoint = (b2MouseJoint *)world->CreateJoint(&mjd);
        body->SetAwake(true);
    }
}

void Renderer::OnMouseMove()
{
    if (mouseJoint)
    {
        mouseWorld = screen2worldb2(sharedData->mousePos);
        mouseJoint->SetTarget(mouseWorld);
    }
}

void Renderer::OnMouseUp()
{
    if (mouseJoint)
    {
        world->DestroyJoint(mouseJoint);
        mouseJoint = nullptr;
    }
}
