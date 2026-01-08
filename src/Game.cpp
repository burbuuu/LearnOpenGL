#include "Game.hpp"
#include "Collision.hpp"
#include "ParticleGenerator.hpp"
#include "PostProcessor.hpp"
#include "engine/ResourceManager.hpp"
#include "BallObject.hpp"
#include "GameObject.hpp"


#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <algorithm>
#include <iostream>


// Constructor
Game::Game(unsigned int width, unsigned int heigth)
    : State(GAME_ACTIVE)
    , Keys()
    , Width(width)
    , Height(heigth)
    , renderer(nullptr)
    , Player(nullptr)
    , Ball(nullptr)
    , Particles(nullptr)
    , Effects(nullptr)
    , ShakeTime(0.0f)
{
}

Game::~Game() 
{
    renderer = nullptr;
}

void Game::Init() 
{
    // Load shaders
    ResourceManager::LoadShader("resources/shaders/sprite.vs", "resources/shaders/sprite.fs", nullptr, "sprite");
    ResourceManager::LoadShader("resources/shaders/particle.vs", "resources/shaders/particle.fs", nullptr, "particle");
    ResourceManager::LoadShader("resources/shaders/post_processing.vs", "resources/shaders/post_processing.fs",nullptr, "postprocessing");

    // Configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), 
        static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    ResourceManager::GetShader("particle").Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("particle").SetMatrix4("projection", projection);


    // Set render-specific controls
    renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
    Effects = new PostProcessor(ResourceManager::GetShader("postprocessing"), this->Width, this->Height);

    // Load textures
    ResourceManager::LoadTexture("resources/textures/awesomeface.png", true, "face"); // Ball texture
    ResourceManager::LoadTexture("resources/textures/background.jpg", false, "background");
    ResourceManager::LoadTexture("resources/textures/block.png", false, "block");
    ResourceManager::LoadTexture("resources/textures/block_solid.png", false, "block_solid");
    ResourceManager::LoadTexture("resources/textures/paddle.png", true, "paddle");
    ResourceManager::LoadTexture("resources/textures/particle.png", true,"particle");

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

    // Initialize particle generator
    Particles = new ParticleGenerator(
        ResourceManager::GetShader("particle"),
        ResourceManager::GetTexture("particle"),
        500
    );

}

void Game::Update(float dt) 
{
    // Update objects
    Ball->Move(dt, this->Width);

    // Check for collisions
    DoCollisions();
    
    // Update particles
    Particles->Update(dt, *Ball, 2, glm::vec2(Ball->radius / 2.0f));

    // Reduce shake time
    if (ShakeTime > 0.0f)
    {
        ShakeTime -= dt;
        if(ShakeTime <= 0.0f) Effects->Shake = false;
    }
    
    // Check loss condition
    if (Ball->Position.y >= this->Height) // did ball reach bottom edge?
    {
        this->ResetLevel();
        this->ResetPlayer();
    }
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


#ifdef DEBUG
    ProcessDebugInput();
#endif
}

void Game::Render() 
{
    if(this->State == GAME_ACTIVE)
    {
        // Begin rendering to postprocessing framebuffer
        Effects->BeginRender();
            // Draw background
            renderer->DrawSprite(ResourceManager::GetTexture("background"),glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
            // Draw level
            Levels[this->Level].Draw(*renderer);
            // Draw player
            Player->Draw(*renderer);
            // Render particles
            Particles->Draw(); // use its own renderer        
            // Render ball
            Ball->Draw(*renderer);
        // End rendering to postprocessing framebuffer
        Effects->EndRender();
        // Render postprocessing quad
        Effects->Render(glfwGetTime());
    } 
}

void Game::ResetLevel()
{
    switch (Level) 
    {
        case 0:
            Levels[0].Load("resources/levels/one.lvl", Width, Height/2);
            break;
        case 1:
            Levels[0].Load("resources/levels/two.lvl", Width, Height/2);
            break;
        case 2:
            Levels[0].Load("resources/levels/three.lvl", Width, Height/2);
            break;
        case 3:
            Levels[0].Load("resources/levels/four.lvl", Width, Height/2);
            break;
        default:
            break;
    }
}

void Game::ResetPlayer()
{
    // Reset player/ball state
    Player->Size = PLAYER_SIZE;
    Player->Position = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
    Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -(BALL_RADIUS * 2.0f)), INITIAL_BALL_VELOCITY);
}

void Game::DoCollisions()
{
    for(GameObject &box : this->Levels[this->Level].Bricks)
    {
        // Skip destroyed boxes
        if(box.Destroyed)
        {
            continue;
        }

        Collision collision = CheckCollision(*Ball, box);
        if (std::get<0>(collision)) // if collision is true
        {
            // destroy block if not solid
            if (!box.IsSolid)   box.Destroyed = true;
            
            // collision resolution
            Direction dir = std::get<1>(collision);
            glm::vec2 diff_vector = std::get<2>(collision);
            if (dir == LEFT || dir == RIGHT) // horizontal collision
            {
                Ball->Velocity.x = -Ball->Velocity.x; // reverse horizontal velocity
                // relocate
                float penetration = Ball->radius - std::abs(diff_vector.x);
                if (dir == LEFT)
                    Ball->Position.x += penetration; // move ball to right
                else
                    Ball->Position.x -= penetration; // move ball to left;
            }
            else // vertical collision
            {
                Ball->Velocity.y = -Ball->Velocity.y; // reverse vertical velocity
                // relocate
                float penetration = Ball->radius - std::abs(diff_vector.y);
                if (dir == UP)
                    Ball->Position.y -= penetration; // move ball bback up
                else
                    Ball->Position.y += penetration; // move ball back down
            }                      
        }
    }

    // Check player collision
    Collision result = CheckCollision(*Ball, *Player);
    if (!Ball->stuck && std::get<0>(result))
    {
        // check where it hit the board, and change velocity based on where it hit the board
        float centerBoard = Player->Position.x + Player->Size.x / 2.0f;
        float distance = (Ball->Position.x + Ball->radius) - centerBoard;
        float percentage = distance / (Player->Size.x / 2.0f);
        // then move accordingly
        float strength = 2.0f;
        glm::vec2 oldVelocity = Ball->Velocity;
        Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength; 
        //Ball->Velocity.y = -Ball->Velocity.y;
        Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity); // keep speed consistent over both axes (multiply by length of old velocity, so total strength is not changed)
        // fix sticky paddle
        Ball->Velocity.y = -1.0f * abs(Ball->Velocity.y);
    }
}


// Debug functions
void Game::ProcessDebugInput()
{
    if (this->State == GAME_ACTIVE)
    {
        // Chaos (toggle) Key I
        if (Keys[GLFW_KEY_I] && !KeysProcessed[GLFW_KEY_I])
        {
            Effects->Chaos = !Effects->Chaos;
            KeysProcessed[GLFW_KEY_I] = true;
        }

        // Confuse (toggle) Key O
        if (Keys[GLFW_KEY_O] && !KeysProcessed[GLFW_KEY_O])
        {
            Effects->Confuse = !Effects->Confuse;
            KeysProcessed[GLFW_KEY_O] = true;
        }

        // Shake (set time to 0.05f): Key P
        if (Keys[GLFW_KEY_P] && !KeysProcessed[GLFW_KEY_P])
        {
            ShakeTime = 0.05f;
            Effects->Shake = true;
            KeysProcessed[GLFW_KEY_P] = true;

        }
    }
}