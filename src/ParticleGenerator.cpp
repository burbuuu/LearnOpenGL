#include "ParticleGenerator.hpp"
#include "GameObject.hpp"
#include "engine/Shader.hpp"
#include "engine/Texture.hpp"
#include "glm/fwd.hpp"

ParticleGenerator::ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount)
    : amount(amount)
    , shader(shader)
    , texture(texture)
{
    this->init();
}

void ParticleGenerator::Update(float dt, GameObject &object, unsigned int newParticles, glm::vec2 offset)
{
    // Add new particles
    for (unsigned int i = 0; i < newParticles; ++i)
    {
        int unusedParticle = this->firstUnusedParticle();
        this->respawnParticle(this->particles[unusedParticle], object, offset);
    }

    // Update all particles
    for (unsigned int i = 0; i < this->amount; ++i)
    {
        Particle &p = particles[i];
        p.Life -= dt;
        if(p.Life > 0.0f) // Update if alive
        {
            p.Position -= p.Velocity *dt;
            p.Color.a -= dt *2.5f;
        }
    }
}

void ParticleGenerator::Draw()
{
    // Use additive blanding to five it a glow effect
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    this->shader.Use();
    for (const Particle & particle : this->particles)
    {
        if (particle.Life > 0.0f)
        {
            this->shader.SetVector2f("offset", particle.Position);
            this->shader.SetVector4f("color", particle.Color);
            this->texture.Bind();
            glBindVertexArray(this->VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }
    }
    // Reset to default blending mode
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleGenerator::init()
{
    /// Set up mesh and attribute properties
    unsigned int VBO;
    float particle_quad[] = 
    {
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(this->VAO);

    // Fill mesh buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);

    // Set mesh atributtes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    // Create amount default particle instances
    particles.reserve(amount);
    for (unsigned int i = 0; i < amount; ++i)
    {
        particles.emplace_back();
    }

    // Initialize lastUsedParticle
    lastUsedParticle = 0;
}

unsigned int ParticleGenerator::firstUnusedParticle()
{
    // First search from last used particle
    for (unsigned int i = lastUsedParticle; i < particles.size(); ++i)
    {
        if(this->particles[i].Life <= 0.0f)
        {
            lastUsedParticle = i;
            return i;
        }
    }

    // Otherwise do a linear search
    for(unsigned int i = 0; i < lastUsedParticle; ++i)
    {
        if(this->particles[i].Life <= 0.0f)
        {
            lastUsedParticle = i;
            return i;
        }
    }

    // If all particles are taken override the first one 
    lastUsedParticle = 0;
    return 0;
}

void ParticleGenerator::respawnParticle(Particle &particle, GameObject &object, glm::vec2 offset)
{
    float random = ((rand() % 100) - 50) / 10.0f;
    float rColor = 0.5f + ((rand() % 100) / 100.0f);
    particle.Position = object.Position + random + offset;
    particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
    particle.Life = 1.0f;
    particle.Velocity = object.Velocity * 0.1f;
}