#include "Screens/Logo.hpp"
#include "Application.hpp"
#include "Screens/Screen.hpp"
#include "engine/PostProcessor.hpp"
#include "engine/SpriteRenderer.hpp"
#include "engine/ResourceManager.hpp"
#include "engine/TextRenderer.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

Logo::Logo(Application *app, unsigned int width, unsigned int height)
    : Screen(app)
    , Width(width)
    , Height(height)
    , level()
{
}

Logo::~Logo()
{
}

void Logo::OnEnter()
{
    Init();
}

void Logo::Init()
{
    level.Load("resources/levels/logo.lvl", Width,Height / 2.0f);
    app->GetContext().sound->playMusic("resources/audio/breakout.wav", true);
}

void Logo::Update(float dt)
{
}

void Logo::ProcessInput(float dt)
{
    Input* input = app->GetContext().input;

    for (int key = 0; key < GLFW_KEY_LAST; ++key)
    {
        if (input->Keys[key] && !input->KeysProcessed[key])
        {
            // Any key pressed for the first time
            input->KeysProcessed[key] = true;
            OnAnyKeyPressed();
            break;
        }
    }
}

void Logo::OnAnyKeyPressed()
{
    app->RequestScreen(ScreenType::Gameplay);
}

void Logo::Render()
{
    SpriteRenderer *renderer = app->GetContext().renderer;
    PostProcessor *effects = app->GetContext().effects;
    TextRenderer *text = app->GetContext().textRender;

    effects->BeginRender();
    {
        renderer->DrawSprite(ResourceManager::GetTexture("background"),glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
        level.Draw(*renderer);
        text->RenderText("Press any button", 250.0f, Height * 3 / 5, 1.0f);


    }
    effects->EndRender();
    effects->Render(glfwGetTime());



}
