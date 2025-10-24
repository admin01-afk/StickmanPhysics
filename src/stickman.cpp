#include <json/json.h>
#include <fstream>
#include "stickman.h"
#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <box2d/box2d.h>

Stickman::Stickman(b2World* world, float x, float y) : world(world), x(x), y(y)
{
    BuildStickman();
}

b2Body* Stickman::createLimb(const Bone& bone){ //const std::string& name, float width, Vector2 start, Vector2 end, const std::string& shape_type = "box") {
    Vector2 diff = {
        bone.end.x - bone.start.x,
        bone.end.y - bone.start.y
    };

    float length = sqrtf(diff.x * diff.x + diff.y * diff.y);
    float angle = atan2f(diff.y, diff.x);

    // center position in world coordinates
    float centerX = (bone.start.x + bone.end.x) / 2.0f;
    float centerY = (bone.start.y + bone.end.y) / 2.0f;

    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    bd.position.Set(centerX, centerY);
    bd.angle = angle;
    b2Body* body = world->CreateBody(&bd);

    b2FixtureDef fd;
    fd.density = 1.0f;
    fd.friction = 0.3f;
    fd.filter.groupIndex = -1;

    if (bone.shape_type == "circle") {
        b2CircleShape shape;
        shape.m_radius = bone.width / 2.0f;
        fd.shape = &shape;
        body->CreateFixture(&fd);
    } else {
        b2PolygonShape shape;
        shape.SetAsBox(bone.width / 2.0f, length / 2.0f);
        fd.shape = &shape;
        body->CreateFixture(&fd);
    }

    bodies.push_back(body);
    std::cout << "Created bone: " << bone.name 
              << " pos(" << centerX << ", " << centerY << ")"
              << " length: " << length << "\n";
    return body;
}

// recursively parses json and returns Bone
Bone Stickman::parseBone(const Json::Value& node) {
    Bone bone;
    bone.name = node["name"].asString();
    bone.width = node["width"].asFloat();

    bone.start = { node["start"]["x"].asFloat(), node["start"]["y"].asFloat() };
    bone.end   = { node["end"]["x"].asFloat(),   node["end"]["y"].asFloat() };
    bone.shape_type = node["shape_type"].asString();

    if (node.isMember("children")) {
        for (const auto& child : node["children"]) {
            bone.children.push_back(parseBone(child));
        }
    }
    bones.push_back(bone);
    return bone;
}

// returns the json file(path) parsed into Bone struct
Bone Stickman::loadStickmanFromFile(const std::string& path) {
    std::ifstream file(path, std::ifstream::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open stickman file: " + path);
    }

    Json::Value root;
    file >> root;
    return parseBone(root["stickman"]);
}

//for debugging bone structure
void printBones(const Bone& bone, int depth = 0) {
    // Indent according to depth (so hierarchy is visible)
    for (int i = 0; i < depth; ++i) std::cout << "\t";

    // Print this bone’s name
    std::cout << bone.name <<"\n";

    // Recurse into children
    for (const auto& child : bone.children) {
        printBones(child, depth + 1);
    }
}

void Stickman::generateLimbs(const Bone& bone, const Vector2& parentPos) {
    // Compute world-space positions for this bone (no rotation applied)
    Vector2 worldStart = {
        parentPos.x + bone.start.x,
        parentPos.y + bone.start.y
    };
    Vector2 worldEnd = {
        parentPos.x + bone.end.x,
        parentPos.y + bone.end.y
    };

    // Copy and update bone with world positions
    Bone worldBone = bone;
    worldBone.start = worldStart;
    worldBone.end = worldEnd;

    // Create this limb (in world coords)
    b2Body* limb = createLimb(worldBone);

    // Create joint to connect this limb to parent
    if (!bodies.empty() && bodies.back() != limb) { // Ensure parent and child bodies are distinct
        b2RevoluteJointDef jointDef;
        jointDef.bodyA = bodies.back(); // Parent body
        jointDef.bodyB = limb;
        jointDef.localAnchorA.Set(
            (worldStart.x - parentPos.x) / scale, 
            (worldStart.y - parentPos.y) / scale
        );
        jointDef.localAnchorB.Set(
            (worldStart.x - worldBone.start.x) / scale, 
            (worldStart.y - worldBone.start.y) / scale
        );
        jointDef.collideConnected = false;
        joints.push_back(world->CreateJoint(&jointDef));
    }

    // Recurse for children
    for (auto& child : bone.children) {
        generateLimbs(child, worldEnd);
    }
}

void Stickman::BuildStickman() {
    // parse json (data.json)
    Bone stickman = loadStickmanFromFile("/home/mehmet/c++/StickmanPhysics/src/data.json");

    //debug
    std::cout << "Bone Structure: \n";
    printBones(stickman);

    // create limbs and add to bodies
    generateLimbs(stickman, {x,y});

    // create joints and add to joints
}