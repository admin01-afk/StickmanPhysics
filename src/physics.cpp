#include "physics.h"

Physics::Ragdoll Physics::CreateStickman(b2Vec2 pos)
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

void Physics::createGround(float x, float y){
    b2BodyDef groundDef;
    groundDef.position.Set(x, y);
    b2Body* ground = world->CreateBody(&groundDef);

    b2PolygonShape groundShape;
    groundShape.SetAsBox(2.0f * x, 0.5f);
    b2FixtureDef groundFD;
    groundFD.shape = &groundShape;
    groundFD.filter.categoryBits = CATEGORY_GROUND;
    groundFD.filter.maskBits = CATEGORY_BACK | CATEGORY_FRONT;
    ground->CreateFixture(&groundFD);
}

void Physics::updatePhysics(){
    if(!isPhysicsPaused) world->Step(1.0f / 60.0f, 8, 3);
}
