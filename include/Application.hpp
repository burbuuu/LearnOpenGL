#pragma once

#include "Screens/Screen.hpp"
#include "engine/SpriteRenderer.hpp"
#include "engine/PostProcessor.hpp"
#include "engine/SoundEngine.hpp"
#include "engine/TextRenderer.hpp"

#include <memory>

struct Context
{
    SpriteRenderer* renderer;
    TextRenderer* textRender;
    PostProcessor* effects;
    SoundEngine* sound;
    struct Input* input;

};

// Small input manager
struct Input
{
    bool Keys[1024];
    bool KeysProcessed[1024];
    Input() { for (int i=0;i<1024;++i){Keys[i]=false; KeysProcessed[i]=false;} }
};

// Available screens
enum class ScreenType
{
    Title
    , Gameplay
};

class Application
{
public:
    Application(unsigned int width, unsigned int height);
    ~Application();

    void Init();
    void SetScreen(std::unique_ptr<Screen> newScreen);
    void RequestScreen(ScreenType type);
    void Update(float dt);
    void Render();

    Context& GetContext() { return context; }

private:

    // Context members (owned by application) 
    std::unique_ptr<SpriteRenderer> renderer;
    std::unique_ptr<TextRenderer> textRender;
    std::unique_ptr<PostProcessor> effects;
    std::unique_ptr<SoundEngine> sound;
    std::unique_ptr<Input> input;

    Context context;
    std::unique_ptr<Screen> currentScreen;

    bool screenChangeRequested = false;
    ScreenType requestedScreen;
    void SwitchScreen(ScreenType requestedScreen);

    unsigned int width;
    unsigned int height;
};
