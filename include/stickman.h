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
    Stickman(b2World* world, float x, float y);
    void BuildStickman();
    b2Body* createLimb(const Bone& bone);
    void generateLimbs(const Bone& bone, const Vector2& parentPos);
    Bone parseBone(const Json::Value& node);
    Bone loadStickmanFromFile(const std::string& path);

    private:
    b2World* world;
    float x, y, scale;
    Vector2 (*b2Pos2screen)(Vector2);
    
    std::vector<b2Body*> bodies;
    std::vector<b2Joint*> joints;
    std::vector<Bone> bones;
};
