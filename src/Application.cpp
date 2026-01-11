#include "Application.hpp"
#include <memory>
#include "engine/PostProcessor.hpp"
#include "engine/ResourceManager.hpp"
#include "engine/SoundEngine.hpp"
#include "engine/TextRenderer.hpp"
#include "Screens/Game.hpp"
#include "Screens/Logo.hpp"

Application::Application(unsigned int w, unsigned int h)
    : width(w)
    , height(h)
{
}

Application::~Application() = default;

void Application::Init()
{
    // Load shaders
    ResourceManager::LoadShader("resources/shaders/sprite.vs", "resources/shaders/sprite.fs", nullptr, "sprite");
    ResourceManager::LoadShader("resources/shaders/particle.vs", "resources/shaders/particle.fs", nullptr, "particle");
    ResourceManager::LoadShader("resources/shaders/post_processing.vs", "resources/shaders/post_processing.fs",nullptr, "postprocessing");
   
    // Configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    ResourceManager::GetShader("particle").Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("particle").SetMatrix4("projection", projection);

    // Create context members and expose them to the context
    renderer = std::make_unique<SpriteRenderer>(ResourceManager::GetShader("sprite"));
    textRender = std::make_unique<TextRenderer>(width, height);
    textRender->Load("resources/fonts/OCRAEXT.TTF", 24);
    effects = std::make_unique<PostProcessor>(ResourceManager::GetShader("postprocessing"),width, height);
    sound = std::make_unique<SoundEngine>();
    input = std::make_unique<Input>();

    context.renderer = renderer.get();
    context.textRender = textRender.get();
    context.effects = effects.get();
    context.sound = sound.get();
    context.input = input.get();



    // Load textures
    ResourceManager::LoadTexture("resources/textures/awesomeface.png", true, "face"); // Ball texture
    ResourceManager::LoadTexture("resources/textures/background.jpg", false, "background");
    ResourceManager::LoadTexture("resources/textures/block.png", false, "block");
    ResourceManager::LoadTexture("resources/textures/block_solid.png", false, "block_solid");
    ResourceManager::LoadTexture("resources/textures/paddle.png", true, "paddle");
    ResourceManager::LoadTexture("resources/textures/particle.png", true,"particle");
    ResourceManager::LoadTexture("resources/textures/powerup_speed.png", true, "powerup_speed");
    ResourceManager::LoadTexture("resources/textures/powerup_sticky.png", true, "powerup_sticky");
    ResourceManager::LoadTexture("resources/textures/powerup_increase.png", true, "powerup_increase");
    ResourceManager::LoadTexture("resources/textures/powerup_confuse.png", true, "powerup_confuse");
    ResourceManager::LoadTexture("resources/textures/powerup_chaos.png", true, "powerup_chaos");
    ResourceManager::LoadTexture("resources/textures/powerup_passthrough.png", true, "powerup_passthrough");

}

void Application::SetScreen(std::unique_ptr<Screen> newScreen)
{
    if (currentScreen)
        currentScreen->OnExit();

    if (newScreen)
    {
        newScreen->OnEnter();
    }

    currentScreen = std::move(newScreen);
}

void Application::RequestScreen(ScreenType type)
{
    screenChangeRequested = true;
    requestedScreen = type;
}

void Application::SwitchScreen(ScreenType requestedScreen)
{
    switch (requestedScreen)
    {

    case ScreenType::Title:
        SetScreen(std::make_unique<Game>(this, width, height));;
        break;

    case ScreenType::Gameplay:
        SetScreen(std::make_unique<Game>(this, width, height));
        break;
    }
}

void Application::Update(float dt)
{
    if (currentScreen)
    {
        currentScreen->ProcessInput(dt);
        currentScreen->Update(dt);
    }

    if (screenChangeRequested)
    {
        SwitchScreen(requestedScreen);
        screenChangeRequested = false;
    }
}

void Application::Render()
{
    if (currentScreen)
        currentScreen->Render();
}

