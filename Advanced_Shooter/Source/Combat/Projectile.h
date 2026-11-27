/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : Projectile.h
Description : Projectile movement and damage
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/

#pragma once

#include "Combat/ProjectileOwner.h"
#include "Objects/GameObject.h"

// basic bullet data
class Projectile : public GameObject
{
public:
    Projectile();

    void Render(sf::RenderWindow& window) override;
    void Update(float fDeltaTime);

    void Initialise(
        const sf::Vector2f& vPosition,
        const sf::Vector2f& vVelocity,
        float fDamage,
        ProjectileOwner eOwner);

    float GetDamage() const;
    ProjectileOwner GetOwner() const;
    sf::Vector2f GetVelocity() const;
    void SetLifeTime(float fLifeTime);

protected:
    sf::Vector2f m_vVelocity;
    float m_fDamage;
    float m_fLifeTime;
    ProjectileOwner m_eOwner;
};
