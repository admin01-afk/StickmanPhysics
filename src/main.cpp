#include "raylib.h"
#include <box2d/box2d.h>
#include <iostream>
#include <vector>

bool DEBUG = true;

const int WIDTH = 1200;
const int HEIGHT = 900;
const float SCALE = 50.0f;

b2MouseJoint *mouseJoint = nullptr;
b2Vec2 mouseWorld;

b2Vec2 gravity(0.0f, -10.0f); // -10
b2World world(gravity);
b2World *worldPtr = &world;
Camera2D camera = {0};

void OnMouseDown();
void OnMouseMove();
void OnMouseUp();

void dPrint(const char *fmt, ...){
    if (DEBUG) {
        printf("[DEBUG] ");
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
    }
}

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

void MousePosDebug()
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

class Renderer
{
public:
    // meters to pixels
    float scale;
    Renderer(float scale) : scale(scale) {}

    void Draw(b2World *world) const
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
                    float radius = circle->m_radius * scale;
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
};
struct Ragdoll
{
    std::vector<b2Body *> limbs;
};

enum CollisionCategory
{
    CATEGORY_TORSO = 0x0001,
    CATEGORY_FRONT = 0x0002,
    CATEGORY_BACK = 0x0004,
    CATEGORY_GROUND = 0x0008
};

Ragdoll CreateStickman(b2World *world, b2Vec2 pos, float scale)
{
    Ragdoll ragdoll;

    auto makeBox = [&](b2Vec2 offset, float hx, float hy, bool dynamic = true,
                       uint16 category = CATEGORY_FRONT, uint16 mask = 0xFFFF)
    {
        b2BodyDef bd;
        bd.type = dynamic ? b2_dynamicBody : b2_staticBody;
        bd.position = pos + offset;
        b2Body *body = world->CreateBody(&bd);

        b2PolygonShape shape;
        shape.SetAsBox(hx, hy);

        b2FixtureDef fd;
        fd.shape = &shape;
        fd.density = 1.0f;
        fd.filter.categoryBits = category;
        fd.filter.maskBits = mask;
        body->CreateFixture(&fd);

        ragdoll.limbs.push_back(body);
        return body;
    };

    auto connect = [&](b2Body *a, b2Body *b, b2Vec2 anchorA, b2Vec2 anchorB)
    {
        b2RevoluteJointDef jd;
        jd.bodyA = a;
        jd.bodyB = b;
        jd.localAnchorA = anchorA;
        jd.localAnchorB = anchorB;
        world->CreateJoint(&jd);
    };

    // --- Torso ---
    b2BodyDef torsoDef;
    torsoDef.type = b2_dynamicBody;
    torsoDef.position = pos + b2Vec2(0, 1.0f);
    b2Body *torso = world->CreateBody(&torsoDef);

    b2PolygonShape torsoShape;
    torsoShape.SetAsBox(0.15f, 0.4f);
    b2FixtureDef torsoFD;
    torsoFD.shape = &torsoShape;
    torsoFD.density = 1.0f;
    torsoFD.filter.categoryBits = 0x0002; // torso category
    torsoFD.filter.maskBits = 0x0002;     // collides only with itself
    torso->CreateFixture(&torsoFD);

    // --- Head ---
    b2BodyDef hd;
    hd.type = b2_dynamicBody;
    hd.position = pos + b2Vec2(0, 1.9f);
    b2Body *head = world->CreateBody(&hd);

    b2CircleShape hshape;
    hshape.m_radius = 0.2f;
    b2FixtureDef headfd;
    headfd.shape = &hshape;
    headfd.density = 1.0f;
    headfd.filter.categoryBits = 0x0004;               // head category
    headfd.filter.maskBits = 0x0004 | CATEGORY_GROUND; // collide with ground
    head->CreateFixture(&headfd);
    ragdoll.limbs.push_back(head);

    connect(torso, head, {0, 0.4f}, {0, -0.2f});

    // --- Legs ---
    b2Vec2 legAnchor = {0, -0.4f};

    b2Body *leftUpperLeg = makeBox(legAnchor, 0.08f, 0.25f, true, CATEGORY_BACK, CATEGORY_BACK | CATEGORY_GROUND);
    b2Body *leftLowerLeg = makeBox({legAnchor.x, legAnchor.y - 0.4f}, 0.08f, 0.25f, true, CATEGORY_BACK, CATEGORY_BACK | CATEGORY_GROUND);
    b2Body *rightUpperLeg = makeBox(legAnchor, 0.08f, 0.25f, true, CATEGORY_FRONT, CATEGORY_FRONT | CATEGORY_GROUND);
    b2Body *rightLowerLeg = makeBox({legAnchor.x, legAnchor.y - 0.4f}, 0.08f, 0.25f, true, CATEGORY_FRONT, CATEGORY_FRONT | CATEGORY_GROUND);

    connect(torso, leftUpperLeg, legAnchor, {0, 0.25f});
    connect(torso, rightUpperLeg, legAnchor, {0, 0.25f});
    connect(leftUpperLeg, leftLowerLeg, {0, -0.25f}, {0, 0.25f});
    connect(rightUpperLeg, rightLowerLeg, {0, -0.25f}, {0, 0.25f});

    // --- Arms ---
    b2Body *leftUpperArm = makeBox({-0.35f, 1.0f}, 0.08f, 0.2f, true, CATEGORY_BACK, CATEGORY_BACK | CATEGORY_GROUND);
    b2Body *leftLowerArm = makeBox({-0.55f, 1.0f}, 0.08f, 0.2f, true, CATEGORY_BACK, CATEGORY_BACK | CATEGORY_GROUND);
    b2Body *rightUpperArm = makeBox({0.35f, 1.0f}, 0.08f, 0.2f, true, CATEGORY_FRONT, CATEGORY_FRONT | CATEGORY_GROUND);
    b2Body *rightLowerArm = makeBox({0.55f, 1.0f}, 0.08f, 0.2f, true, CATEGORY_FRONT, CATEGORY_FRONT | CATEGORY_GROUND);

    connect(torso, leftUpperArm, {-0.15f, 0.3f}, {0, 0.2f});
    connect(torso, rightUpperArm, {0.15f, 0.3f}, {0, 0.2f});
    connect(leftUpperArm, leftLowerArm, {0, -0.2f}, {0, 0.2f});
    connect(rightUpperArm, rightLowerArm, {0, -0.2f}, {0, 0.2f});

    return ragdoll;
}

b2BodyDef grounddef;
b2Body *ground = nullptr;
Vector2 mousePos;

int main()
{
    InitWindow(WIDTH, HEIGHT, "Stickman Ragdoll");
    SetTargetFPS(60);
    camera.offset = {WIDTH / 2.0f, HEIGHT / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    Vector2 target = {WIDTH / 2.0f, HEIGHT / 2.0f};

    Renderer renderer(SCALE);

    grounddef.type = b2_staticBody;
    grounddef.position.Set(WIDTH / SCALE / 2.0f, -(HEIGHT / SCALE) + 0.5f);
    dPrint("Ground position: (%.2f, %.2f)\n", grounddef.position.x, grounddef.position.y);
    ground = world.CreateBody(&grounddef);
    b2PolygonShape boxshape;
    boxshape.SetAsBox((WIDTH / SCALE / 2.0f) + 10, 0.5f);
    b2FixtureDef groundFD;
    groundFD.shape = &boxshape;
    groundFD.density = 1.0f;
    groundFD.filter.categoryBits = CATEGORY_GROUND;
    groundFD.filter.maskBits = CATEGORY_BACK | CATEGORY_FRONT;
    ground->CreateFixture(&groundFD);

    Ragdoll stickman = CreateStickman(&world, {WIDTH / SCALE / 2, -HEIGHT / SCALE / 2}, SCALE);

    bool physicsPaused = false;
    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_F1))
            DEBUG = !DEBUG;

        if (IsKeyDown(KEY_D))
            target.x += 200 * GetFrameTime();
        if (IsKeyDown(KEY_A))
            target.x -= 200 * GetFrameTime();
        if (IsKeyDown(KEY_S))
            target.y += 200 * GetFrameTime();
        if (IsKeyDown(KEY_W))
            target.y -= 200 * GetFrameTime();

        // zoom
        float zoomSpeed = 1.0f; // zoom units per second
        float dt = GetFrameTime();

        if (IsKeyDown(KEY_Q))
            camera.zoom += zoomSpeed * dt * camera.zoom;
        if (IsKeyDown(KEY_E))
            camera.zoom -= zoomSpeed * dt * camera.zoom;
        if (camera.zoom > 5.0f)
            camera.zoom = 5.0f; // max zoom in
        if (camera.zoom < 0.1f)
            camera.zoom = 0.1f; // max zoom out

        camera.target = target;

        mousePos = GetMousePosition();
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            OnMouseDown();
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            OnMouseMove();
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            OnMouseUp();

        if (IsKeyPressed(KEY_P))
            physicsPaused = !physicsPaused; // toggle pause
        if (!physicsPaused)
            world.Step(1.0f / 60.0f, 8, 3);
        BeginDrawing();
        BeginMode2D(camera);
        ClearBackground(BLACK);
        if (DEBUG){
            for (int i = 0; i <= WIDTH / SCALE; i++)
            {
                Color c = (i == 0) ? RED : DARKGRAY;
                DrawLineV({i * SCALE + 1, 0}, {i * SCALE, HEIGHT}, c);
            }
            for (int j = 0; j <= HEIGHT / SCALE; j++)
            {
                Color c = (j == 0) ? BLUE : DARKGRAY;
                DrawLineV({0, j * SCALE}, {WIDTH, j * SCALE}, c);
            }
        }
        renderer.Draw(&world);
        EndMode2D();
        MousePosDebug();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}

void OnMouseDown()
{
    mouseWorld = screen2worldb2(mousePos);
    dPrint("Mouse World pos: (%.2f, %.2f)\n", mouseWorld.x, mouseWorld.y);

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
    worldPtr->QueryAABB(&callback, aabb);

    if (callback.fixture)
    {
        b2Body *body = callback.fixture->GetBody();
        b2MouseJointDef mjd;
        mjd.bodyA = ground;
        mjd.bodyB = body;
        mjd.target = mouseWorld;
        mjd.maxForce = 1000.0f * body->GetMass();
        mjd.stiffness = 50.0f;
        mjd.damping = 5.0f;
        mouseJoint = (b2MouseJoint *)worldPtr->CreateJoint(&mjd);
        body->SetAwake(true);
    }
}

void OnMouseMove()
{
    if (mouseJoint)
    {
        mouseWorld = screen2worldb2(mousePos);
        mouseJoint->SetTarget(mouseWorld);
    }
}

void OnMouseUp()
{
    if (mouseJoint)
    {
        worldPtr->DestroyJoint(mouseJoint);
        mouseJoint = nullptr;
    }
}