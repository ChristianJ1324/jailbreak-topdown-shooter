/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : Player.h
Description : Player movement and shooting
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#pragma once

#include "Characters/Character.h"
#include "Combat/PistolWeapon.h"
#include "Combat/Projectile.h"
#include <memory>
#include <vector>

// player controls and aim live here
class Player : public Character
{
public:
    Player();

    void HandleInput(const sf::RenderWindow& window);
    void Update(float fDeltaTime) override;
    void Render(sf::RenderWindow& window) override;

    void Fire(std::vector<std::unique_ptr<Projectile>>& projectiles);

    void SetCheckpointPosition(const sf::Vector2f& vCheckpointPosition);
    sf::Vector2f GetCheckpointPosition() const;

    void SetInvincible(bool bIsInvincible);
    bool IsInvincible() const;

    sf::Vector2f GetAimDirection() const;
    sf::Vector2f GetMuzzlePosition() const;

    void RefreshDashCharges();
    void SetDashCharges(int iDashCharges);
    int GetDashCharges() const;
    int GetMaxDashCharges() const;
    bool WantsBulletTime() const;
    void SetBulletTimeMeter(float fBulletTimeMeter);
    float GetBulletTimeMeter() const;
    float GetBulletTimeMeterPercent() const;
    void SetSpriteTexture(const sf::Texture* pTexture);

private:
    void UpdateDash(float fDeltaTime);
    void TryStartDash(const sf::Vector2f& vMoveInput);

    PistolWeapon m_pistolWeapon;
    const sf::Texture* m_pSpriteTexture;
    sf::Vector2f m_vAimDirection;
    sf::Vector2f m_vDashDirection;
    sf::Vector2f m_vCheckpointPosition;
    int m_iDashCharges;
    bool m_bIsInvincible;
    bool m_bWantsBulletTime;
    bool m_bWasDashKeyDown;
    bool m_bWasFireButtonDown;
    bool m_bIsFireHeld;
    bool m_bIsDashing;
    float m_fDashTimer;
    float m_fDashRechargeTimer;
    float m_fBulletTimeMeter;
};
