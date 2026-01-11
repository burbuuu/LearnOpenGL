#include "Screens/Game.hpp"
#include "Collision.hpp"
#include "ParticleGenerator.hpp"
#include "PowerUp.hpp"
#include "engine/ResourceManager.hpp"
#include "BallObject.hpp"
#include "GameObject.hpp"
#include "Application.hpp"
#include "engine/PostProcessor.hpp"
#include "engine/SpriteRenderer.hpp"
#include "engine/SoundEngine.hpp"
#include "engine/TextRenderer.hpp"


#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <algorithm>
#include <cstdlib>
#include <sstream>



// Constructor
Game::Game(Application *application, unsigned int width, unsigned int heigth)
    : Screen(application)
    , State(GAME_MENU)
    , Width(width)
    , Height(heigth)
    , Player(nullptr)
    , lives(3)
    , Ball(nullptr)
    , Particles(nullptr)
    , ShakeTime(0.0f)
{
}

Game::~Game() 
{
    // renderer/effects/sound are owned by Application (access via app->GetContext())
    Player = nullptr;
    Ball = nullptr;
    Particles = nullptr;
    Levels.clear();
}

void Game::Init() 
{
    // Application is responsible for loading shaders/textures and creating renderer/effects/sound and text renderer. We acces it 
    // throught app->context

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
    app->GetContext().sound->playMusic("resources/audio/breakout.wav", true);
}

void Game::OnEnter()
{
    Init();
}

void Game::Update(float dt) 
{
    // Update objects
    Ball->Move(dt, this->Width);

    // Check for collisions
    DoCollisions();
    
    // Update particles
    Particles->Update(dt, *Ball, 2, glm::vec2(Ball->Radius / 2.0f));

    // PowerUps
    UpdatePowerUps(dt);

    // Reduce shake time
    if (ShakeTime > 0.0f)
    {
        ShakeTime -= dt;
        if(ShakeTime <= 0.0f) app->GetContext().effects->Shake = false;
    }
    
    CheckLossLives();
    CheckLevelCompletion();
}

void Game::ProcessInput(float dt) 
{
    // Get input
    Input* input = app->GetContext().input;

    if (this->State == GAME_MENU)
    {
        if (input->Keys[GLFW_KEY_ENTER] && !input->KeysProcessed[GLFW_KEY_ENTER])
        {
            StartGame();
            input->KeysProcessed[GLFW_KEY_ENTER] = true;
        }
        if (input->Keys[GLFW_KEY_W] && !input->KeysProcessed[GLFW_KEY_W])
        {
            this->Level = (this->Level + 1) % 4;
            input->KeysProcessed[GLFW_KEY_W] = true;
        }
        if (input->Keys[GLFW_KEY_S] && !input->KeysProcessed[GLFW_KEY_S])
        {
            if (this->Level > 0)
                --this->Level;
            else
                this->Level = 3;
            input->KeysProcessed[GLFW_KEY_S] = true;
        }
    }
    if (State == GAME_ACTIVE)
    {   
        // Define gameplay input here
        float velocity = PLAYER_VELOCITY * dt;
        float oldX = Player->Position.x;

        // Move player
        if (input->Keys[GLFW_KEY_A])
            Player->Position.x -= velocity;

        if (input->Keys[GLFW_KEY_D])
            Player->Position.x += velocity;

        // Clamp X position
        Player->Position.x = std::clamp(Player->Position.x,0.0f,this->Width - Player->Size.x);

        // Move ball with the player if is stuck
        if(Ball->Stuck)
        {
            float deltaX = Player->Position.x - oldX;
            Ball->Position.x += deltaX;
        }

        if(input->Keys[GLFW_KEY_SPACE])
        {
            Ball->Stuck = false;
        }
    }
    if (this->State == GAME_WIN)
    {
        if (input->Keys[GLFW_KEY_ENTER])
        {
            input->KeysProcessed[GLFW_KEY_ENTER] = true;
            app->GetContext().effects->Chaos = false;
            this->State = GAME_MENU;
        }
    }


#ifdef DEBUG
    ProcessDebugInput();
#endif
}

void Game::Render() 
{
    SpriteRenderer *renderer = app->GetContext().renderer;
    PostProcessor *effects = app->GetContext().effects;
    TextRenderer *text = app->GetContext().textRender;

    if(this->State == GAME_ACTIVE || this->State == GAME_MENU || this->State == GAME_WIN)
    {
        // Begin rendering to postprocessing framebuffer
        effects->BeginRender();
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
            // Power ups
            for (PowerUp &powerUp : this->PowerUps)
            {
                if (!powerUp.Destroyed)
                    powerUp.Draw(*renderer);
            }
        // End rendering to postprocessing framebuffer
        effects->EndRender();
        // Render postprocessing quad
        effects->Render(glfwGetTime());
        
        // Display lives
        std::stringstream ss; ss << this->lives;
        text->RenderText("Lives:" + ss.str(), 5.0f, 5.0f, 1.0f);
    }
    if (this->State == GAME_MENU)
    {
        text->RenderText("Press ENTER to start", 250.0f, Height / 2, 1.0f);
        text->RenderText("Press W or S to select level", 245.0f, Height / 2 + 20.0f, 0.75f);
    }
    if (this->State == GAME_WIN)
    {
        text->RenderText("You WON!!!", 320.0, Height / 2 - 20.0, 1.0, glm::vec3(0.0, 1.0, 0.0)
        );
        text->RenderText("Press ENTER to retry or ESC to quit", 130.0, Height / 2, 1.0, glm::vec3(1.0, 1.0, 0.0)
        );
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
            Levels[1].Load("resources/levels/two.lvl", Width, Height/2);
            break;
        case 2:
            Levels[2].Load("resources/levels/three.lvl", Width, Height/2);
            break;
        case 3:
            Levels[3].Load("resources/levels/four.lvl", Width, Height/2);
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

    // Disable all active powerups
    app->GetContext().effects->Chaos = false;
    app->GetContext().effects->Confuse = false;
    Player->Color = glm::vec3(1.0f);
    Ball->Color = glm::vec3(1.0f);
}

void Game::StartGame()
{
    lives = 3;
    State = GAME_ACTIVE;
    ResetLevel();
    ResetPlayer();
}

void Game::CheckLossLives()
{
    // did ball reach bottom edge?
    if (Ball->Position.y >= this->Height)
    {
        lives--;
        ResetPlayer();
    }

    if (lives <= 0)
    {
        ResetLevel();
        State = GAME_MENU;
    }
}

void Game::CheckLevelCompletion()
{
    if (!Levels[Level].IsCompleted() || State != GAME_ACTIVE) return;

    this->ResetLevel();
    this->ResetPlayer();
    app->GetContext().effects->Chaos = true;
    this->State = GAME_WIN;
}

void Game::DoCollisions()
{
    for (GameObject &box : this->Levels[this->Level].Bricks)
    {
        if (!box.Destroyed)
        {
            Collision collision = CheckCollision(*Ball, box);
            if (std::get<0>(collision)) // if collision is true
            {
                // destroy block if not solid
                if (!box.IsSolid)
                {
                    box.Destroyed = true;
                    this->SpawnPowerUps(box);
                    app->GetContext().sound->playSFX("resources/audio/blip.wav");
                }
                else
                {   // if block is solid, enable shake effect
                    ShakeTime = 0.05f;
                    app->GetContext().effects->Shake = true;
                    // Play solid block sound
                    app->GetContext().sound->playSFX("resources/audio/solid.wav");
                }
                // collision resolution
                Direction dir = std::get<1>(collision);
                glm::vec2 diff_vector = std::get<2>(collision);
                if (!(Ball->PassThrough && !box.IsSolid)) // don't do collision resolution on non-solid bricks if pass-through is activated
                {
                    if (dir == LEFT || dir == RIGHT) // horizontal collision
                    {
                        Ball->Velocity.x = -Ball->Velocity.x; // reverse horizontal velocity
                        // relocate
                        float penetration = Ball->Radius - std::abs(diff_vector.x);
                        if (dir == LEFT)
                            Ball->Position.x += penetration; // move ball to right
                        else
                            Ball->Position.x -= penetration; // move ball to left;
                    }
                    else // vertical collision
                    {
                        Ball->Velocity.y = -Ball->Velocity.y; // reverse vertical velocity
                        // relocate
                        float penetration = Ball->Radius - std::abs(diff_vector.y);
                        if (dir == UP)
                            Ball->Position.y -= penetration; // move ball bback up
                        else
                            Ball->Position.y += penetration; // move ball back down
                    }
                }
            }
        }    
    }

    // Check player collision
    Collision result = CheckCollision(*Ball, *Player);
    if (!Ball->Stuck && std::get<0>(result))
    {
        // check where it hit the board, and change velocity based on where it hit the board
        float centerBoard = Player->Position.x + Player->Size.x / 2.0f;
        float distance = (Ball->Position.x + Ball->Radius) - centerBoard;
        float percentage = distance / (Player->Size.x / 2.0f);
        // then move accordingly
        float strength = 2.0f;
        glm::vec2 oldVelocity = Ball->Velocity;
        Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength; 
        //Ball->Velocity.y = -Ball->Velocity.y;
        Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity); // keep speed consistent over both axes (multiply by length of old velocity, so total strength is not changed)
        // fix sticky paddle
        Ball->Velocity.y = -1.0f * abs(Ball->Velocity.y);

        // If ball is sticky, stuck it to the paddle
        Ball->Stuck = Ball->Sticky; // Implements sticky effect

        // Play sfx
        app->GetContext().sound->playSFX("resources/audio/bleep.wav");

    }

    // Check for power up / player interaction
    for (PowerUp& powerUp : this->PowerUps)
    {
        if (powerUp.Destroyed) continue; // Don't interact with destroyed powerups

        if (powerUp.Position.y >= this->Height) 
        {
            powerUp.Destroyed = true; // Flag out of bounds powerups as destroyed
        }
        if (CheckCollision(*Player, powerUp))
        { // Collided with player, activate powerup
            ActivatePowerUp(powerUp);
            powerUp.Destroyed = true;
            powerUp.Activated = true;

            // Play sfx
            app->GetContext().sound->playSFX("resources/audio/powerup.wav");
        }
    }
}


// Debug functions
void Game::ProcessDebugInput()
{
    if (this->State == GAME_ACTIVE)
    {
        // Chaos (toggle) Key I
        if (app->GetContext().input->Keys[GLFW_KEY_I] && !app->GetContext().input->KeysProcessed[GLFW_KEY_I])
        {
            app->GetContext().effects->Chaos = !app->GetContext().effects->Chaos;
            app->GetContext().input->KeysProcessed[GLFW_KEY_I] = true;
        }

        // Confuse (toggle) Key O
        if (app->GetContext().input->Keys[GLFW_KEY_O] && !app->GetContext().input->KeysProcessed[GLFW_KEY_O])
        {
            app->GetContext().effects->Confuse = !app->GetContext().effects->Confuse;
            app->GetContext().input->KeysProcessed[GLFW_KEY_O] = true;
        }

        // Shake (set time to 0.05f): Key P
        if (app->GetContext().input->Keys[GLFW_KEY_P] && !app->GetContext().input->KeysProcessed[GLFW_KEY_P])
        {
            ShakeTime = 0.05f;
            app->GetContext().effects->Shake = true;
            app->GetContext().input->KeysProcessed[GLFW_KEY_P] = true;
        }

        // Destroy all blocks: key U
        if (app->GetContext().input->Keys[GLFW_KEY_U] &&
            !app->GetContext().input->KeysProcessed[GLFW_KEY_U])
        {
            for (GameObject& brick : Levels[Level].Bricks)
            {
                if (!brick.IsSolid)
                    brick.Destroyed = true;
            }

            app->GetContext().input->KeysProcessed[GLFW_KEY_U] = true;
        }
    }
}

// Power up spawn and update functions

bool Game::ShouldSpawn(unsigned int chance)
{
    unsigned int random = rand() % chance;
    return random == 0;
}

void Game::SpawnPowerUps(GameObject &block)
{
    if (ShouldSpawn(75)) // 1 in 75 chance
        this->PowerUps.push_back(PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, block.Position, ResourceManager::GetTexture("powerup_speed")));
    if (ShouldSpawn(75))
        this->PowerUps.push_back(PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, block.Position, ResourceManager::GetTexture("powerup_sticky")));
    if (ShouldSpawn(75))
        this->PowerUps.push_back(PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, block.Position, ResourceManager::GetTexture("powerup_passthrough")));
    if (ShouldSpawn(75))
        this->PowerUps.push_back(PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4), 0.0f, block.Position, ResourceManager::GetTexture("powerup_increase")));
    if (ShouldSpawn(15)) // Negative powerups should spawn more often
        this->PowerUps.push_back(PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, block.Position, ResourceManager::GetTexture("powerup_confuse")));
    if (ShouldSpawn(15))
        this->PowerUps.push_back(PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, block.Position, ResourceManager::GetTexture("powerup_chaos")));
}

void Game::ActivatePowerUp(PowerUp& powerUp)
{
    if (powerUp.Type == "speed")
    {
        Ball->Velocity *= 1.2;
    }
    else if (powerUp.Type == "sticky")
    {
        Ball->Sticky = true;
        Player->Color = glm::vec3(1.0f, 0.5f, 1.0f);
    }
    else if (powerUp.Type == "pass-through")
    {
        Ball->PassThrough = true;
        Ball->Color = glm::vec3(1.0f, 0.5f, 0.5f);
    }
    else if (powerUp.Type == "pad-size-increase")
    {
        Player->Size.x += 50;
    }
    else if (powerUp.Type == "confuse")
    {
                    if (!app->GetContext().effects->Chaos)
                        app->GetContext().effects->Confuse = true; // only activate if chaos wasn't already active
    }
    else if (powerUp.Type == "chaos")
    {
                    if (!app->GetContext().effects->Confuse)
                        app->GetContext().effects->Chaos = true;
    }
}

void Game::UpdatePowerUps(float dt)
{
    for (PowerUp &powerUp : this->PowerUps)
    {
        powerUp.Position += powerUp.Velocity * dt;
        if (powerUp.Activated)
        {
            powerUp.Duration -= dt;

            if (powerUp.Duration <= 0.0f)
            {
                // remove powerup from list (will later be removed)
                powerUp.Activated = false;
                // deactivate effects
                if (powerUp.Type == "sticky")
                {
                    if (!IsOtherPowerUpActive(this->PowerUps, "sticky"))
                    {	// only reset if no other PowerUp of type sticky is active
                        Ball->Sticky = false;
                        Player->Color = glm::vec3(1.0f);
                    }
                }
                else if (powerUp.Type == "pass-through")
                {
                    if (!IsOtherPowerUpActive(this->PowerUps, "pass-through"))
                    {	// only reset if no other PowerUp of type pass-through is active
                        Ball->PassThrough = false;
                        Ball->Color = glm::vec3(1.0f);
                    }
                }
                else if (powerUp.Type == "confuse")
                {
                    if (!IsOtherPowerUpActive(this->PowerUps, "confuse"))
                    {	// only reset if no other PowerUp of type confuse is active
                        app->GetContext().effects->Confuse = false;
                    }
                }
                else if (powerUp.Type == "chaos")
                {
                    if (!IsOtherPowerUpActive(this->PowerUps, "chaos"))
                    {	// only reset if no other PowerUp of type chaos is active
                        app->GetContext().effects->Chaos = false;
                    }
                }
            }
        }
    }
    // Remove all PowerUps from vector that are destroyed AND !activated (thus either off the map or finished)
    // Note we use a lambda expression to remove each PowerUp which is destroyed and not activated
    this->PowerUps.erase(std::remove_if(this->PowerUps.begin(), this->PowerUps.end(),
        [](const PowerUp &powerUp) { return powerUp.Destroyed && !powerUp.Activated; }
    ), this->PowerUps.end());
}

bool Game::IsOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type)
{
    // Check if another PowerUp of the same type is still active
    // in which case we don't disable its effect (yet)
    for (const PowerUp &powerUp : powerUps)
    {
        if (powerUp.Activated)
            if (powerUp.Type == type)
                return true;
    }
    return false;
}
