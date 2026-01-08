#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

#include "engine/Shader.hpp"
#include "engine/Texture.hpp"
#include "GameObject.hpp"

struct Particle
{
    glm::vec2 Position, Velocity;
    glm::vec4 Color;
    float Life;

    Particle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) { }
};

class ParticleGenerator
{
public:
    //Constructor
    ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount);
    
    void Update(float dt, GameObject &object, unsigned int newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
    void Draw();

private:
    // State
    std::vector<Particle> particles;
    unsigned int amount;
    unsigned int lastUsedParticle;

    // Render state
    Shader shader;
    Texture2D texture;
    unsigned int VAO;

    // Initialize buffer and vertex attributes
    void init();

    unsigned int firstUnusedParticle();
    void respawnParticle(Particle &particle, GameObject &object, glm::vec2 offset = glm::vec2(0.0f ,0.0f));
};