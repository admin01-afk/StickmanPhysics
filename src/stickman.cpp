/*
Stickman
    parse json and build stickman(boxes, joints)
    ...
*/
#include <json/json.h>
#include <fstream>
#include "stickman.h"
#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <box2d/box2d.h>

Stickman::Stickman(b2World* world, float x, float y, float scale) : world(world), x(x), y(y), scale(scale)
{
    BuildStickman();
}

// Refactor params to Bone 
b2Body* Stickman::createLimb(const Bone& bone){ //const std::string& name, float width, Vector2 start, Vector2 end, const std::string& shape_type = "box") {
    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    bd.position.Set(x, y); // temporary, will be adjusted by parent
    b2Body* body = world->CreateBody(&bd);

    b2FixtureDef fd;
    fd.density = 1.0f;
    fd.friction = 0.3f;
    fd.filter.groupIndex = -1;

    if (bone.shape_type == "circle") {
        auto* circle = new b2CircleShape();
        circle->m_radius = bone.width; // assume width = radius
        fd.shape = circle;
        body->CreateFixture(&fd);
        delete circle;
    } else {
        auto* box = new b2PolygonShape();
        box->SetAsBox(bone.width / 2.0f, //width
                      sqrt(pow((bone.end.x - bone.start.x),2) + pow((bone.end.y - bone.start.y),2)), //height
                      {(bone.layer == "middle") ? 0 : ((bone.layer == "left") ? bone.width/2 : -bone.width/2 ),0}, //center
                      (bone.layer == "middle") ? 0 : ((bone.layer == "left") ? 90 : -90 ) // angle
                    ); // Box2D uses half-sizes
        fd.shape = box;
        body->CreateFixture(&fd);
        delete box;
    }
    std::cout << "Generated body: " << bone.name << "\n";
    bodies.push_back(body);
    return body;
}

// recursively parses json and returns Bone
Bone parseBone(const Json::Value& node) {
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
    return bone;
}

// returns the json file(path) parsed into Bone struct
Bone loadStickmanFromFile(const std::string& path) {
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

void Stickman::generateLimbs(Bone& bone) {
    createLimb(bone);
    for (auto& child : bone.children) {
        generateLimbs(child);
    }
}

void Stickman::BuildStickman() {
    // parse json (data.json)
    Bone stickman = loadStickmanFromFile("/home/mehmet/c++/StickmanPhysics/src/data.json");

    //debug
    std::cout << "Bone Structure: \n";
    printBones(stickman);

    // create limbs and add to bodies
    generateLimbs(stickman);

    // create joints and add to joints
}

void Stickman::Draw(float scale) {
    for (auto* b : bodies) {
        b2Fixture* fixture = b->GetFixtureList();
        if (!fixture) continue;

        b2Shape::Type type = fixture->GetType();
        b2Vec2 p = b->GetPosition();
        float a = b->GetAngle();

        if (type == b2Shape::e_circle) {
            b2CircleShape* c = (b2CircleShape*)fixture->GetShape();
            Vector2 screenPos = { p.x * scale + 400, 600 - p.y * scale };
            DrawCircleV(screenPos, c->m_radius * scale, GREEN);
        } else if (type == b2Shape::e_polygon) {
            DrawRectanglePro(
                { p.x * scale + 400, 600 - p.y * scale, 0.4f * scale, 1.0f * scale },
                { 0.2f * scale, 0.5f * scale },
                -a * RAD2DEG,
                RED
            );
        }
    }
}