#pragma once
#include "EventBus.h"
#include "Events.h"
#include <box2d/box2d.h>
#include "SharedData.h"

#include <vector>

class Physics {
public:
    Physics(SharedData* sharedData) : sharedData(sharedData) {
        // Initialize Box2D world
        b2Vec2 gravity(0.0f, -10.0f);
        world = new b2World(gravity);
        sharedData->world = world;

        EventBus::Subscribe<PausePhysicsEvent>([this](const PausePhysicsEvent& e) {
            isPhysicsPaused = e.isPaused;
        });
    }

    ~Physics() {
        delete world;
    }

    enum CollisionCategory {
        CATEGORY_TORSO = 0x0001,
        CATEGORY_FRONT = 0x0002,
        CATEGORY_BACK = 0x0004,
        CATEGORY_GROUND = 0x0008
    };

    struct Ragdoll {
        std::vector<b2Body*> limbs;
    };
    
    Ragdoll CreateStickman(b2Vec2 pos);
    void updatePhysics();
    void createGround(float x, float y); // consider ground pos based on window size

private:
    b2World* world;
    SharedData* sharedData;
    bool isPhysicsPaused = false;
};