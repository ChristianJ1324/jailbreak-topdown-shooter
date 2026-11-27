/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : Enemy.cpp
Description : Enemy health, shooting, and UI
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#include "Characters/Enemy.h"
#include "Characters/Player.h"
#include "Core/VectorMath.h"
#include <algorithm>

Enemy::Enemy(EnemyBehaviour eBehaviour)
    : m_eBehaviour(eBehaviour)
    , m_fAttackTimer(0.f)
    , m_pSpriteTexture(nullptr)
    , m_vFacingDirection(1.f, 0.f)
    , m_bIsAimingAtPlayer(false)
{
    SetSize(sf::Vector2f(28.f, 28.f));
    SetMaxHealth(24.f);
    SetHealth(24.f);
    m_fMoveSpeed = m_eBehaviour == EnemyBehaviour::KeepDistance ? 100.f : 145.f;
}

void Enemy::UpdateAI(
    const Player& player,
    std::vector<std::unique_ptr<Projectile>>& projectiles,
    bool bCanSeePlayer)
{
    // behaviour picked by level tile
    if (m_eBehaviour == EnemyBehaviour::KeepDistance)
    {
        UpdateKeepDistanceAI(player, projectiles, bCanSeePlayer);
        return;
    }

    UpdateCloseRangeAI(player, projectiles, bCanSeePlayer);
}

void Enemy::Update(float fDeltaTime)
{
    if (m_fAttackTimer > 0.f)
    {
        m_fAttackTimer -= fDeltaTime;
    }

    if (!m_bIsAimingAtPlayer)
    {
        // keep facing target while shooting
        SetFacingDirection(m_vVelocity);
    }

    UpdateWalkFrame(fDeltaTime, 6, 0.095f);
    Character::Update(fDeltaTime);
    m_bIsAimingAtPlayer = false;
}

void Enemy::Render(sf::RenderWindow& window)
{
    DrawWalkingSprite(window, m_pSpriteTexture, m_vFacingDirection, 6);

    // only show health bar after damage
    if (m_fHealth >= m_fMaxHealth || !IsAlive())
    {
        return;
    }

    const sf::FloatRect rectBounds = GetBounds();
    const sf::Vector2f vCenter = rectBounds.position + (rectBounds.size * 0.5f);
    const sf::Vector2f vBarPosition(
        vCenter.x - 17.f,
        rectBounds.position.y - 8.f);
    const float fHealthPercent = std::max(0.f, m_fHealth / m_fMaxHealth);

    sf::RectangleShape shapeBarBack(sf::Vector2f(34.f, 4.f));
    shapeBarBack.setPosition(vBarPosition);
    shapeBarBack.setFillColor(sf::Color(35, 10, 15, 230));
    window.draw(shapeBarBack);

    sf::RectangleShape shapeBarFill(sf::Vector2f(34.f * fHealthPercent, 4.f));
    shapeBarFill.setPosition(vBarPosition);
    shapeBarFill.setFillColor(sf::Color(230, 70, 75));
    window.draw(shapeBarFill);
}

void Enemy::FireAtPlayer(const Player& player, std::vector<std::unique_ptr<Projectile>>& projectiles)
{
    const sf::Vector2f vDirection = VectorMath::GetDirection(player.GetPosition() - GetPosition());

    if (VectorMath::IsZero(vDirection))
    {
        return;
    }

    // face player before firing
    SetFacingDirection(vDirection);
    m_bIsAimingAtPlayer = true;

    if (m_fAttackTimer > 0.f)
    {
        return;
    }

    const sf::Vector2f vStartPosition =
        GetPosition() + (GetSize() * 0.5f) - sf::Vector2f(16.f, 16.f);

    std::unique_ptr<Projectile> pProjectile = std::make_unique<Projectile>();

    pProjectile->SetSize(sf::Vector2f(
        32.f, 
        32.f));

    pProjectile->Initialise(
        vStartPosition,
        vDirection * 420.f,
        25.f,
        ProjectileOwner::Enemy);

    pProjectile->SetLifeTime(2.2f);
    projectiles.push_back(std::move(pProjectile));
    m_fAttackTimer = 2.4f;
}

bool Enemy::IsLowHealth() const
{
    return m_fHealth <= (m_fMaxHealth * 0.35f);
}

void Enemy::UpdateCloseRangeAI(
    const Player& player,
    std::vector<std::unique_ptr<Projectile>>& projectiles,
    bool bCanSeePlayer)
{
    const sf::Vector2f vDirection = player.GetPosition() - GetPosition();
    const float fDistance = VectorMath::Length(vDirection);
    const sf::Vector2f vNormal = VectorMath::GetDirection(vDirection);

    // close enemy movement
    if (!bCanSeePlayer)
    {
        SetVelocity(vNormal * m_fMoveSpeed);
        SetFacingDirection(vNormal);
    }
    else if (IsLowHealth() || fDistance < 150.f)
    {
        SetVelocity(-vNormal * m_fMoveSpeed);
    }
    else if (fDistance > 260.f)
    {
        SetVelocity(vNormal * m_fMoveSpeed);
    }
    else
    {
        SetVelocity(sf::Vector2f(0.f, 0.f));
    }

    if (bCanSeePlayer)
    {
        FireAtPlayer(player, projectiles);
    }
}

void Enemy::UpdateKeepDistanceAI(
    const Player& player,
    std::vector<std::unique_ptr<Projectile>>& projectiles,
    bool bCanSeePlayer)
{
    const sf::Vector2f vDirection = player.GetPosition() - GetPosition();
    const float fDistance = VectorMath::Length(vDirection);
    const sf::Vector2f vNormal = VectorMath::GetDirection(vDirection);

    // distance enemy movement
    if (!bCanSeePlayer)
    {
        SetVelocity(vNormal * m_fMoveSpeed);
        SetFacingDirection(vNormal);
    }
    else if (IsLowHealth() || fDistance < 220.f)
    {
        SetVelocity(-vNormal * m_fMoveSpeed);
    }
    else if (fDistance > 430.f)
    {
        SetVelocity(vNormal * m_fMoveSpeed);
    }
    else
    {
        SetVelocity(sf::Vector2f(0.f, 0.f));
    }

    if (bCanSeePlayer && fDistance <= 430.f)
    {
        FireAtPlayer(player, projectiles);
    }
}

void Enemy::SetSpriteTexture(const sf::Texture* pTexture)
{
    m_pSpriteTexture = pTexture;
}

void Enemy::SetAttackDelay(float fAttackDelay)
{
    m_fAttackTimer = fAttackDelay;
}

void Enemy::SetFacingDirection(const sf::Vector2f& vDirection)
{
    const sf::Vector2f vNormal = VectorMath::GetDirection(vDirection);

    if (VectorMath::IsZero(vNormal))
    {
        return;
    }

    m_vFacingDirection = vNormal;
}
