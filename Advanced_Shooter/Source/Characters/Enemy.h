/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : Enemy.h
Description : Enemy health, shooting, and UI
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#pragma once

#include "Characters/Character.h"
#include "Combat/Projectile.h"
#include <memory>
#include <vector>

class Player;

enum class EnemyBehaviour
{
    CloseRange,
    KeepDistance
};

// one enemy class with a behaviour setting
class Enemy : public Character
{
public:
    explicit Enemy(EnemyBehaviour eBehaviour);

    void UpdateAI(
        const Player& player,
        std::vector<std::unique_ptr<Projectile>>& projectiles,
        bool bCanSeePlayer);

    void Update(float fDeltaTime) override;
    void Render(sf::RenderWindow& window) override;
    void SetSpriteTexture(const sf::Texture* pTexture);
    void SetAttackDelay(float fAttackDelay);

private:
    void UpdateCloseRangeAI(
        const Player& player,
        std::vector<std::unique_ptr<Projectile>>& projectiles,
        bool bCanSeePlayer);
    void UpdateKeepDistanceAI(
        const Player& player,
        std::vector<std::unique_ptr<Projectile>>& projectiles,
        bool bCanSeePlayer);
    bool IsLowHealth() const;
    void FireAtPlayer(const Player& player, std::vector<std::unique_ptr<Projectile>>& projectiles);
    void SetFacingDirection(const sf::Vector2f& vDirection);

    EnemyBehaviour m_eBehaviour;
    float m_fAttackTimer;
    const sf::Texture* m_pSpriteTexture;
    sf::Vector2f m_vFacingDirection;
    bool m_bIsAimingAtPlayer;
};
