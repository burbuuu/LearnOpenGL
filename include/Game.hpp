#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

#include "BallObject.hpp"
#include "GameObject.hpp"
#include "ParticleGenerator.hpp"
#include "PowerUp.hpp"
#include "Screens/Screen.hpp"
#include "GameLevel.hpp"


// forward declarations for context-provided services
class SpriteRenderer;
class SoundEngine;
class PostProcessor;


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


class Game : public Screen
{
public:
    GameState State;
    unsigned int Width, Height;
    
    // Constructor and destructor
    Game(Application *app, unsigned int width, unsigned int height);
    ~Game();

    // Initialize game state (load shaders, textures and levels)
    void Init();

    // Screen lifecycle
    void OnEnter() override;

    // Game loop
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();

    SpriteRenderer* GetRenderer();

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
    // renderer is provided by Application via Screen::app

    // Levels
    std::vector<GameLevel> Levels;
    unsigned int Level;

    // Player
    GameObject* Player;

    // Ball
    BallObject* Ball;

    // Particles
    ParticleGenerator *Particles;

    // Post processor (provided by Application)
    float ShakeTime;

    // Power ups
    std::vector<PowerUp> PowerUps;

    // Sound engine provided by Application

    void CheckLevelCompletion();
    void CheckLossLives();
};