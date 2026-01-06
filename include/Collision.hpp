#pragma once

#include <tuple>
#include <glm/glm.hpp>

class GameObject;
class BallObject;

enum Direction
{
    UP,
    RIGHT,
    DOWN,
    LEFT
};

// Define a Collision typedef that represents collision data
typedef std::tuple<bool, Direction, glm::vec2> Collision; // <collision?, what direction?, difference vector center - closest point>


// Define collision detection methods
bool CheckCollision(GameObject &one, GameObject&two);
Collision CheckCollision(BallObject &one, GameObject &two);
Direction VectorDirection(glm::vec2 closest);

