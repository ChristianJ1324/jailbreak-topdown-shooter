/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : Character.h
Description : Shared character health and movement
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#pragma once

#include "Objects/GameObject.h"

// shared health movement and walk sprites
class Character : public GameObject
{
public:
    Character();

    virtual void Update(float fDeltaTime);

    void TakeDamage(float fDamage);
    void Heal(float fAmount);
    void Die();

    bool IsAlive() const;
    float GetHealth() const;
    float GetMaxHealth() const;

    void SetHealth(float fHealth);
    void SetMaxHealth(float fMaxHealth);
    void SetMoveSpeed(float fMoveSpeed);
    float GetMoveSpeed() const;

    sf::Vector2f GetVelocity() const;
    void SetVelocity(const sf::Vector2f& vVelocity);

protected:
    void UpdateWalkFrame(float fDeltaTime, int iFrameCount, float fFrameTime);
    void DrawWalkingSprite(
        sf::RenderWindow& window,
        const sf::Texture* pTexture,
        const sf::Vector2f& vDirection,
        int iFrameCount) const;

    float m_fHealth;
    float m_fMaxHealth;
    float m_fMoveSpeed;
    sf::Vector2f m_vVelocity;
    float m_fWalkAnimationTimer;
    int m_iWalkAnimationFrame;
    bool m_bIsAlive;
};
