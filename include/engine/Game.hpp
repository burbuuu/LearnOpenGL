#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

enum GameState
{
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

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
};