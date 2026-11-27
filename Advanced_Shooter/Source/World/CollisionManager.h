/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : CollisionManager.h
Description : Collision checks and wall resolution
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#pragma once

#include "Objects/Wall.h"
#include <vector>

// shared box collision checks
class CollisionManager
{
public:
    static bool CheckCollision(const GameObject& objectA, const GameObject& objectB);

    static void ResolveWallCollision(
        GameObject& movingObject,
        const std::vector<Wall>& walls,
        sf::Vector2f& vVelocity);
};
