#pragma once

#include <glm/glm.hpp>

#include "GameObject.hpp"
#include "engine/Texture.hpp"

class BallObject : public GameObject
{
public:
    // Ball state
    float radius;
    bool stuck;

    BallObject();
    BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite);

    glm::vec2 Move(float dt, unsigned int window_width);
    void Reset(glm::vec2 position, glm::vec2 velocity);
};