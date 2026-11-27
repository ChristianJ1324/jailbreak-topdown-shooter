/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : CollisionManager.cpp
Description : Collision checks and wall resolution
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#include "World/CollisionManager.h"
#include <optional>

bool CollisionManager::CheckCollision(const GameObject& objectA, const GameObject& objectB)
{
    // inactive objects skip collision
    if (!objectA.IsActive() || !objectB.IsActive())
    {
        return false;
    }

    return objectA.GetBounds().findIntersection(objectB.GetBounds()).has_value();
}

void CollisionManager::ResolveWallCollision(
    GameObject& movingObject,
    const std::vector<Wall>& walls,
    sf::Vector2f& vVelocity)
{
    // basic box wall pushback
    for (const Wall& wallCurrent : walls)
    {
        if (!wallCurrent.IsActive())
        {
            continue;
        }

        const std::optional<sf::FloatRect> optOverlap =
            movingObject.GetBounds().findIntersection(wallCurrent.GetBounds());

        if (!optOverlap.has_value())
        {
            continue;
        }

        const sf::FloatRect rectMoving = movingObject.GetBounds();
        const sf::FloatRect rectWall = wallCurrent.GetBounds();
        const sf::Vector2f vMovingCenter = rectMoving.position + (rectMoving.size * 0.5f);
        const sf::Vector2f vWallCenter = rectWall.position + (rectWall.size * 0.5f);

        // push out on the smallest overlap
        if (optOverlap->size.x < optOverlap->size.y)
        {
            const float fPushX = vMovingCenter.x < vWallCenter.x ? -optOverlap->size.x : optOverlap->size.x;
            movingObject.MoveBy(sf::Vector2f(fPushX, 0.f));
            vVelocity.x = 0.f;
        }
        else
        {
            const float fPushY = vMovingCenter.y < vWallCenter.y ? -optOverlap->size.y : optOverlap->size.y;
            movingObject.MoveBy(sf::Vector2f(0.f, fPushY));
            vVelocity.y = 0.f;
        }
    }
}
