#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include "GameObject.hpp"
#include "engine/Texture.hpp"
#include "glm/fwd.hpp"

const glm::vec2 SIZE(60.0f, 20.0f);
const glm::vec2 VELOCITY(0.0f, 150.0f);

class PowerUp : public GameObject
{
public:
    // Powerup state
    std::string Type;
    float Duration;
    bool Activated;

    // Constructor
    PowerUp(std::string type, glm::vec3 color, float duration, glm::vec2 postion, Texture2D texture);
};

