#include <engine/Game.hpp>

// Constructor
Game::Game(unsigned int width, unsigned int heigth)
    : State(GAME_ACTIVE)
    , Keys()
    , Width(width)
    , Height(heigth)
{
}

Game::~Game() {}

void Game::Init() {}

void Game::Update(float dt) {}

void Game::ProcessInput(float dt) {}

void Game::Render() {}
