#include <Game.hpp>
#include <engine/ResourceManager.hpp>
#include "BallObject.hpp"
#include "GLFW/glfw3.h"
#include "GameObject.hpp"
#include "glm/fwd.hpp"
#include <algorithm>


// Constructor
Game::Game(unsigned int width, unsigned int heigth)
    : State(GAME_ACTIVE)
    , Keys()
    , Width(width)
    , Height(heigth)
    , renderer(nullptr)
    , Player(nullptr)
    , Ball(nullptr)
{
}

Game::~Game() 
{
    renderer = nullptr;
}

void Game::Init() 
{
    // load shaders
    ResourceManager::LoadShader("resources/shaders/sprite.vs", "resources/shaders/sprite.fs", nullptr, "sprite");
    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), 
        static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    // set render-specific controls
    renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

    // load textures
    ResourceManager::LoadTexture("resources/textures/awesomeface.png", true, "face"); // Ball texture
    ResourceManager::LoadTexture("resources/textures/background.jpg", false, "background");
    ResourceManager::LoadTexture("resources/textures/block.png", false, "block");
    ResourceManager::LoadTexture("resources/textures/block_solid.png", false, "block_solid");
    ResourceManager::LoadTexture("resources/textures/paddle.png", true, "paddle");

    // Load levels
    GameLevel one, two, three, four;
    one.Load("resources/levels/one.lvl", this->Width, this->Height / 2);
    two.Load("resources/levels/two.lvl", this->Width, this->Height / 2);
    three.Load("resources/levels/three.lvl", this->Width, this->Height / 2);
    four.Load("resources/levels/four.lvl", this->Width, this->Height / 2);

    this->Levels.push_back(one);
    this->Levels.push_back(two);
    this->Levels.push_back(three);
    this->Levels.push_back(four);
    this->Level = 0;

    // Initialize player
    glm::vec2 playerInitialPos = glm::vec2(Width/2.0f - PLAYER_SIZE.x/2.0f, Height- PLAYER_SIZE.y);
    Player = new GameObject(playerInitialPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

    // Initialize ball
    glm::vec2 ballPos = playerInitialPos + glm::vec2(PLAYER_SIZE.x/2.0f - BALL_RADIUS, -BALL_RADIUS *2.0f);
    Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));
}

void Game::Update(float dt) 
{
    Ball->Move(dt, this->Width);
}

void Game::ProcessInput(float dt) 
{
    if (State == GAME_ACTIVE)
    {   
        // Define gameplay input here
        float velocity = PLAYER_VELOCITY * dt;
        float oldX = Player->Position.x;

        // Move player
        if (Keys[GLFW_KEY_A])
            Player->Position.x -= velocity;

        if (Keys[GLFW_KEY_D])
            Player->Position.x += velocity;

        // Clamp X position
        Player->Position.x = std::clamp(Player->Position.x,0.0f,this->Width - Player->Size.x);

        // Move ball with the player if is stuck
        if(Ball->stuck)
        {
            float deltaX = Player->Position.x - oldX;
            Ball->Position.x += deltaX;
        }

        if(Keys[GLFW_KEY_SPACE])
        {
            Ball->stuck = false;
        }
    }
}

void Game::Render() 
{
    if(this->State == GAME_ACTIVE)
    {
        // Draw background
        renderer->DrawSprite(ResourceManager::GetTexture("background"),glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
        
        // Draw level
        this->Levels[this->Level].Draw(*renderer);

        // Draw player
        Player->Draw(*renderer);

        // Render ball
        Ball->Draw(*renderer);
    }
}
