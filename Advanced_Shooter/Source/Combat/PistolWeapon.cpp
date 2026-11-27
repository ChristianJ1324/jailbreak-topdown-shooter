/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : PistolWeapon.cpp
Description : Player pistol firing control
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#include "Combat/PistolWeapon.h"

PistolWeapon::PistolWeapon()
    : m_fCooldownTimer(0.f)
{
}

void PistolWeapon::Update(float fDeltaTime)
{
    if (m_fCooldownTimer > 0.f)
    {
        // tick down cooldown
        m_fCooldownTimer -= fDeltaTime;
    }
}

void PistolWeapon::Fire(
    std::vector<std::unique_ptr<Projectile>>& projectiles,
    const sf::Vector2f& vStartPosition,
    const sf::Vector2f& vDirection)
{
    // faster tap fire
    if (m_fCooldownTimer > 0.f)
    {
        return;
    }

    // convert muzzle centre to bullet position
    std::unique_ptr<Projectile> pBullet = std::make_unique<Projectile>();
    pBullet->SetSize(sf::Vector2f(12.f, 12.f));
    pBullet->Initialise(
        vStartPosition - sf::Vector2f(6.f, 6.f),
        vDirection * 680.f,
        8.f,
        ProjectileOwner::Player);
    pBullet->SetLifeTime(1.35f);
    projectiles.push_back(std::move(pBullet));

    // cooldown starts after shot spawns
    m_fCooldownTimer = 0.08f;
}
