#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

#include "BallObject.hpp"
#include "GameObject.hpp"
#include "ParticleGenerator.hpp"
#include "PowerUp.hpp"
#include "engine/SoundEngine.hpp"
#include "engine/SpriteRenderer.hpp"
#include "GameLevel.hpp"
#include "PostProcessor.hpp"


enum GameState
{
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

// Player contsants
const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
const float PLAYER_VELOCITY(500.0f);

// Ball constants
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
const float BALL_RADIUS = 12.5f;


class Game
{
public:
    GameState State;
    bool Keys[1024];
    bool KeysProcessed[1024]; // Tracks if the key press has been handled. Used for single press action 
    unsigned int Width, Height;
    
    // Constructor and destructor
    Game(unsigned int width, unsigned int height);
    ~Game();

    // Initialize game state (load shaders, textures and levels)
    void Init();

    // Game loop
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();

    SpriteRenderer* GetRenderer(){ return renderer; }

    // Reset
    void ResetLevel();
    void ResetPlayer();

    void DoCollisions();

    // Debug
    void ProcessDebugInput();

    // Power ups
    bool ShouldSpawn(unsigned int change);
    void SpawnPowerUps(GameObject &block);
    void UpdatePowerUps(float dt);
    void ActivatePowerUp(PowerUp &powerUp);
    bool IsOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type);

    

private:
    SpriteRenderer* renderer;

    // Levels
    std::vector<GameLevel> Levels;
    unsigned int Level;

    // Player
    GameObject* Player;

    // Ball
    BallObject* Ball;

    // Particles
    ParticleGenerator *Particles;

    // Post processor 
    PostProcessor *Effects;
    float ShakeTime;

    // Power ups
    std::vector<PowerUp> PowerUps;

    SoundEngine *soundEngine;

    void CheckLevelCompletion();
    void CheckLossCondition();
};