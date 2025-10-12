#pragma once
#include <box2d/box2d.h>
#include <vector>
#include "raylib.h"
#include <string>
#include <unordered_map>
#include <json/json.h>

struct Bone {
    std::string name;
    Vector2 start;
    Vector2 end;
    std::string shape_type;
    float width;
    std::string layer;
    std::vector<Bone> children;
};

class Stickman {
public:
    Stickman(b2World* world, float x, float y, float scale = 1.0f);
    void BuildStickman();
    void Draw(float scale);
    b2Body* createLimb(const Bone& bone);
    void generateLimbs(Bone& bone);
private:
    // Member variables
    b2World* world;
    float x, y, scale;

    // Bodies and joints
    std::vector<b2Body*> bodies;
    std::vector<b2Joint*> joints;
};
