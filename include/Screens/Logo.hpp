#pragma once

#include "GameLevel.hpp"
#include "Screens/Screen.hpp"

class Logo : public Screen
{
public:
    unsigned int Width, Height;
    
    // Constructor and destructor
    Logo(Application *app, unsigned int width, unsigned int height);
    ~Logo();

    // Initialize logo
    void Init();

    // Screen lifecycle
    void OnEnter() override;

    // Game loop
    void ProcessInput(float dt) override;
    void Update(float dt) override;
    void Render() override;
private:
    GameLevel level;
    void OnAnyKeyPressed();
};

