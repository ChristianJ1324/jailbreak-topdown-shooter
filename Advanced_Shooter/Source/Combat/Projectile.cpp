/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : Projectile.cpp
Description : Projectile movement and damage
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#include "Combat/Projectile.h"
#include "Core/VectorMath.h"

Projectile::Projectile()
    : m_vVelocity(0.f, 0.f)
    , m_fDamage(10.f)
    , m_fLifeTime(2.f)
    , m_eOwner(ProjectileOwner::Player)
{
}

void Projectile::Render(sf::RenderWindow& window)
{
    if (!m_bIsActive || !m_sprite.has_value())
    {
        return;
    }

    // rotate bullet sprite to match velocity direction
    sf::Sprite spriteProjectile = *m_sprite;
    const sf::Vector2u vTextureSize = spriteProjectile.getTexture().getSize();

    if (vTextureSize.x > 0 && vTextureSize.y > 0)
    {
        const sf::Vector2f vCenter = GetPosition() + (GetSize() * 0.5f);
        spriteProjectile.setOrigin(sf::Vector2f(
            static_cast<float>(vTextureSize.x) * 0.5f,
            static_cast<float>(vTextureSize.y) * 0.5f));
        spriteProjectile.setPosition(vCenter);

        if (!VectorMath::IsZero(m_vVelocity))
        {
            spriteProjectile.setRotation(sf::radians(VectorMath::GetAngle(m_vVelocity)));
        }
    }

    window.draw(spriteProjectile);
}

void Projectile::Update(float fDeltaTime)
{
    if (!m_bIsActive)
    {
        return;
    }

    // remove missed shots after a bit
    m_fLifeTime -= fDeltaTime;
    MoveBy(m_vVelocity * fDeltaTime);

    if (m_fLifeTime <= 0.f)
    {
        SetActive(false);
    }
}

void Projectile::Initialise(
    const sf::Vector2f& vPosition,
    const sf::Vector2f& vVelocity,
    float fDamage,
    ProjectileOwner eOwner)
{
    // same start values for player and enemy bullets
    SetPosition(vPosition);
    m_vVelocity = vVelocity;
    m_fDamage = fDamage;
    m_eOwner = eOwner;
    m_fLifeTime = 2.f;
    SetActive(true);
}

float Projectile::GetDamage() const
{
    return m_fDamage;
}

ProjectileOwner Projectile::GetOwner() const
{
    return m_eOwner;
}

sf::Vector2f Projectile::GetVelocity() const
{
    return m_vVelocity;
}

void Projectile::SetLifeTime(float fLifeTime)
{
    m_fLifeTime = fLifeTime;
}
