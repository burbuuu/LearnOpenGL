#pragma once

#include <GameObject.hpp>
#include <engine/SpriteRenderer.hpp>
#include <vector>

class GameLevel {
public:
    std::vector<GameObject> Bricks;
    // constructor
    GameLevel() { }
    // Load level from file
    void Load(const char *file, unsigned int levelWidth, unsigned int levelHeight);
    // Render level
    void Draw(SpriteRenderer &renderer);
    // Check if the level is completed
    bool IsCompleted();
private:
    void init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight);
};