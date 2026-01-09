#include "PowerUp.hpp"

PowerUp::PowerUp(std::string type, glm::vec3 color, float duration, glm::vec2 position, Texture2D texture) 
    : GameObject(position, SIZE, texture, color, VELOCITY)
    , Type(type)
    , Duration(duration)
    , Activated()
{
}