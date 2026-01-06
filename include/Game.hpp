#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "BallObject.hpp"
#include "GameObject.hpp"
#include "engine/SpriteRenderer.hpp"
#include "GameLevel.hpp"

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
    

private:
    SpriteRenderer* renderer;

    // Levels
    std::vector<GameLevel> Levels;
    unsigned int Level;

    // Player
    GameObject* Player;

    // Ball
    BallObject* Ball;
};