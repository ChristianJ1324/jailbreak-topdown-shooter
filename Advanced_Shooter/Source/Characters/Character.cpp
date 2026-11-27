/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : Character.cpp
Description : Shared character health and movement
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#include "Characters/Character.h"
#include "Core/VectorMath.h"
#include <algorithm>

Character::Character()
    : m_fHealth(100.f)
    , m_fMaxHealth(100.f)
    , m_fMoveSpeed(220.f)
    , m_vVelocity(0.f, 0.f)
    , m_fWalkAnimationTimer(0.f)
    , m_iWalkAnimationFrame(0)
    , m_bIsAlive(true)
{
}

void Character::Update(float fDeltaTime)
{
    if (!m_bIsAlive)
    {
        return;
    }

    MoveBy(m_vVelocity * fDeltaTime);
}

void Character::TakeDamage(float fDamage)
{
    if (!m_bIsAlive)
    {
        return;
    }

    m_fHealth = std::max(0.f, m_fHealth - fDamage);

    if (m_fHealth <= 0.f)
    {
        Die();
    }
}

void Character::Heal(float fAmount)
{
    if (fAmount <= 0.f)
    {
        return;
    }

    // dont heal past max
    m_fHealth = std::min(m_fMaxHealth, m_fHealth + fAmount);
}

void Character::Die()
{
    m_bIsAlive = false;
    m_bIsActive = false;
}

bool Character::IsAlive() const
{
    return m_bIsAlive;
}

float Character::GetHealth() const
{
    return m_fHealth;
}

float Character::GetMaxHealth() const
{
    return m_fMaxHealth;
}

void Character::SetHealth(float fHealth)
{
    // health also controls active state
    m_fHealth = std::clamp(fHealth, 0.f, m_fMaxHealth);
    m_bIsAlive = m_fHealth > 0.f;
    m_bIsActive = m_bIsAlive;
}

void Character::SetMaxHealth(float fMaxHealth)
{
    m_fMaxHealth = std::max(1.f, fMaxHealth);
    m_fHealth = std::min(m_fHealth, m_fMaxHealth);
}

void Character::SetMoveSpeed(float fMoveSpeed)
{
    m_fMoveSpeed = fMoveSpeed;
}

float Character::GetMoveSpeed() const
{
    return m_fMoveSpeed;
}

sf::Vector2f Character::GetVelocity() const
{
    return m_vVelocity;
}

void Character::SetVelocity(const sf::Vector2f& vVelocity)
{
    m_vVelocity = vVelocity;
}

void Character::UpdateWalkFrame(float fDeltaTime, int iFrameCount, float fFrameTime)
{
    if (VectorMath::IsZero(m_vVelocity))
    {
        m_fWalkAnimationTimer = 0.f;
        m_iWalkAnimationFrame = 0;
        return;
    }

    // shared walk frame timing
    m_fWalkAnimationTimer += fDeltaTime;

    while (m_fWalkAnimationTimer >= fFrameTime)
    {
        m_fWalkAnimationTimer -= fFrameTime;
        m_iWalkAnimationFrame = (m_iWalkAnimationFrame + 1) % iFrameCount;
    }
}

void Character::DrawWalkingSprite(
    sf::RenderWindow& window,
    const sf::Texture* pTexture,
    const sf::Vector2f& vDirection,
    int iFrameCount) const
{
    if (pTexture == nullptr || !IsActive())
    {
        return;
    }

    const sf::Vector2u vTextureSize = pTexture->getSize();

    if (vTextureSize.x == 0 || vTextureSize.y == 0)
    {
        return;
    }

    // horizontal walk strip
    const int iFrameWidth = static_cast<int>(vTextureSize.x) / iFrameCount;
    const int iFrameHeight = static_cast<int>(vTextureSize.y);
    const int iFrameIndex = std::clamp(m_iWalkAnimationFrame, 0, iFrameCount - 1);

    if (iFrameWidth <= 0 || iFrameHeight <= 0)
    {
        return;
    }

    sf::Sprite spriteCharacter(*pTexture);
    spriteCharacter.setTextureRect(sf::IntRect(
        sf::Vector2i(iFrameWidth * iFrameIndex, 0),
        sf::Vector2i(iFrameWidth, iFrameHeight)));
    spriteCharacter.setOrigin(sf::Vector2f(
        static_cast<float>(iFrameWidth) * 0.5f,
        static_cast<float>(iFrameHeight) * 0.5f));
    spriteCharacter.setPosition(GetPosition() + (GetSize() * 0.5f));
    spriteCharacter.setRotation(sf::radians(VectorMath::GetAngle(vDirection)));
    window.draw(spriteCharacter);
}
