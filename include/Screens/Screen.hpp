#pragma once

#include <stdexcept>

class Application;

class Screen
{
public:
    explicit Screen(Application *a) : app(a) { if (!app) throw std::runtime_error("Screen requires non-null Application pointer"); }
    virtual ~Screen() = default;

    virtual void OnEnter() {}
    virtual void OnExit() {}

    virtual void ProcessInput(float dt) = 0;
    virtual void Update(float dt) = 0;
    virtual void Render() = 0;

protected:
    Application *const app;
};