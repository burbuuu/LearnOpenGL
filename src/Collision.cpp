#include "Collision.hpp"
#include <tuple>
#include "GameObject.hpp"
#include "BallObject.hpp"
#include "glm/common.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"

bool CheckCollision(GameObject &one, GameObject &two) // AABB - AABB collision
{
    // Collision on the x-axis
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x && two.Position.x + two.Size.x >= one.Position.x;

    // Collision on the y-axis
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y && two.Position.y + two.Size.y >= one.Position.y;

    return collisionX && collisionY;
}

Collision CheckCollision(BallObject &one, GameObject &two)
{
    // Get center point circle first
    glm::vec2 center(one.Position + one.Radius);
    // Calculate AABB info (center, half extents)
    glm::vec2 aabbHalfExtents(two.Size.x / 2.0f, two.Size.y/2.0f);
    glm::vec2 aabbCenter(
        two.Position.x + aabbHalfExtents.x,
        two.Position.y + aabbHalfExtents.y
    );
    // Get difference vector between both centers
    glm::vec2 difference = center - aabbCenter;
    glm::vec2 clamped = glm::clamp(difference, -aabbHalfExtents, aabbHalfExtents);
    // Add clamped value to aabbCenter and we get value of box closest to circle
    glm::vec2 closest = aabbCenter + clamped;
    // Retrieve vector between center circle and closest point AABB and check if length <= radius
    difference = closest - center;

    if(glm::length(difference) <= one.Radius)
        return std::make_tuple(true, VectorDirection(difference),difference);
    else
        return std::make_tuple(false, UP, glm::vec2(0.0f,0.0f));

}

// Calculates wich direction
Direction VectorDirection(glm::vec2 target)
{
    glm::vec2 compass[] = {
        glm::vec2(0.0f, 1.0f), // up
        glm::vec2(1.0f, 0.0f), // right
        glm::vec2(0.0f, -1.0f), // left
        glm::vec2(-1.0f, 0.0f) // down
    };

    float max = 0.0f;
    unsigned int bestMatch = -1;
    for(unsigned int i=0; i < 4; i++)
    {
        float dotProduct = glm::dot(glm::normalize(target), compass[i]);
        if (dotProduct > max)
        {
            max = dotProduct;
            bestMatch = i;
        }
    }
    return (Direction)bestMatch;
}