/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : PistolWeapon.h
Description : Player pistol firing control
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#pragma once

#include "Combat/Projectile.h"
#include <memory>
#include <vector>

// pistol cooldown and bullet spawning
class PistolWeapon
{
public:
    PistolWeapon();

    void Update(float fDeltaTime);
    void Fire(
        std::vector<std::unique_ptr<Projectile>>& projectiles,
        const sf::Vector2f& vStartPosition,
        const sf::Vector2f& vDirection);

private:
    float m_fCooldownTimer;
};
