/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : Player.cpp
Description : Player movement and shooting
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#include "Characters/Player.h"
#include "Core/VectorMath.h"
#include <algorithm>

Player::Player()
    : m_pSpriteTexture(nullptr)
    , m_vAimDirection(1.f, 0.f)
    , m_vDashDirection(1.f, 0.f)
    , m_vCheckpointPosition(64.f, 64.f)
    , m_iDashCharges(3)
    , m_bIsInvincible(false)
    , m_bWantsBulletTime(false)
    , m_bWasDashKeyDown(false)
    , m_bWasFireButtonDown(false)
    , m_bIsFireHeld(false)
    , m_bIsDashing(false)
    , m_fDashTimer(0.f)
    , m_fDashRechargeTimer(0.f)
    , m_fBulletTimeMeter(1.f)
{
    SetSize(sf::Vector2f(28.f, 28.f));
    SetMaxHealth(100.f);
    SetHealth(100.f);
}

void Player::HandleInput(const sf::RenderWindow& window)
{
    // raw input first so diagonal movement works right
    sf::Vector2f vMoveInput(0.f, 0.f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
    {
        vMoveInput.y -= 1.f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
    {
        vMoveInput.y += 1.f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
    {
        vMoveInput.x -= 1.f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
    {
        vMoveInput.x += 1.f;
    }

    if (!m_bIsDashing)
    {
        m_vVelocity = VectorMath::GetDirection(vMoveInput) * m_fMoveSpeed;
    }

    const sf::Vector2i vMousePixels = sf::Mouse::getPosition(window);
    const sf::Vector2f vMouseWorld = window.mapPixelToCoords(vMousePixels);
    const sf::Vector2f vCenter = GetPosition() + (GetSize() * 0.5f);
    m_vAimDirection = VectorMath::GetDirectionOrRight(vMouseWorld - vCenter);

    const bool bMouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
    const bool bBulletTimeMouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);
    m_bWantsBulletTime = bBulletTimeMouseDown && m_fBulletTimeMeter > 0.02f;
    m_bIsFireHeld = bMouseDown && !m_bWasFireButtonDown;
    m_bWasFireButtonDown = bMouseDown;

    const bool bDashKeyDown = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);

    if (bDashKeyDown && !m_bWasDashKeyDown)
    {
        TryStartDash(vMoveInput);
    }

    m_bWasDashKeyDown = bDashKeyDown;
}

void Player::Update(float fDeltaTime)
{
    // bullet time meter
    if (m_bWantsBulletTime)
    {
        m_fBulletTimeMeter = std::max(0.f, m_fBulletTimeMeter - (0.24f * fDeltaTime));

        if (m_fBulletTimeMeter <= 0.f)
        {
            m_bWantsBulletTime = false;
        }
    }
    else
    {
        m_fBulletTimeMeter = std::min(1.f, m_fBulletTimeMeter + (0.32f * fDeltaTime));
    }

    m_pistolWeapon.Update(fDeltaTime);
    UpdateDash(fDeltaTime);
    UpdateWalkFrame(fDeltaTime, 6, 0.085f);
    Character::Update(fDeltaTime);
}

void Player::Render(sf::RenderWindow& window)
{
    DrawWalkingSprite(window, m_pSpriteTexture, m_vAimDirection, 6);
}

void Player::Fire(std::vector<std::unique_ptr<Projectile>>& projectiles)
{
    // pistol handles the actual shot
    if (!m_bIsFireHeld)
    {
        return;
    }

    m_pistolWeapon.Fire(projectiles, GetMuzzlePosition(), m_vAimDirection);
}

void Player::SetCheckpointPosition(const sf::Vector2f& vCheckpointPosition)
{
    m_vCheckpointPosition = vCheckpointPosition;
}

sf::Vector2f Player::GetCheckpointPosition() const
{
    return m_vCheckpointPosition;
}

void Player::SetInvincible(bool bIsInvincible)
{
    m_bIsInvincible = bIsInvincible;
}

bool Player::IsInvincible() const
{
    return m_bIsInvincible;
}

sf::Vector2f Player::GetAimDirection() const
{
    return m_vAimDirection;
}

sf::Vector2f Player::GetMuzzlePosition() const
{
    const sf::Vector2f vCenter = GetPosition() + (GetSize() * 0.5f);
    return vCenter + (m_vAimDirection * 24.f);
}

void Player::RefreshDashCharges()
{
    m_iDashCharges = 3;
    m_fDashRechargeTimer = 0.f;
}

void Player::SetDashCharges(int iDashCharges)
{
    m_iDashCharges = std::clamp(iDashCharges, 0, 3);
    m_fDashRechargeTimer = 0.f;
}

int Player::GetDashCharges() const
{
    return m_iDashCharges;
}

int Player::GetMaxDashCharges() const
{
    return 3;
}

bool Player::WantsBulletTime() const
{
    return m_bWantsBulletTime;
}

void Player::SetBulletTimeMeter(float fBulletTimeMeter)
{
    m_fBulletTimeMeter = std::clamp(fBulletTimeMeter, 0.f, 1.f);
}

float Player::GetBulletTimeMeter() const
{
    return m_fBulletTimeMeter;
}

float Player::GetBulletTimeMeterPercent() const
{
    return std::clamp(m_fBulletTimeMeter, 0.f, 1.f);
}

void Player::SetSpriteTexture(const sf::Texture* pTexture)
{
    m_pSpriteTexture = pTexture;
}

void Player::UpdateDash(float fDeltaTime)
{
    if (m_bIsDashing)
    {
        // dash velocity burst
        m_fDashTimer -= fDeltaTime;
        m_vVelocity = m_vDashDirection * 720.f;

        if (m_fDashTimer <= 0.f)
        {
            m_bIsDashing = false;
            m_vVelocity = sf::Vector2f(0.f, 0.f);
        }
    }

    if (m_iDashCharges >= 3)
    {
        m_fDashRechargeTimer = 0.f;
        return;
    }

    m_fDashRechargeTimer += fDeltaTime;

    if (m_fDashRechargeTimer >= 1.15f)
    {
        m_fDashRechargeTimer = 0.f;
        ++m_iDashCharges;
    }
}

void Player::TryStartDash(const sf::Vector2f& vMoveInput)
{
    if (m_iDashCharges <= 0 || m_bIsDashing)
    {
        return;
    }

    // dash toward aim if not moving
    const sf::Vector2f vMoveDirection = VectorMath::GetDirection(vMoveInput);
    m_vDashDirection = (vMoveDirection.x != 0.f || vMoveDirection.y != 0.f) ? vMoveDirection : m_vAimDirection;
    m_bIsDashing = true;
    m_fDashTimer = 0.14f;
    --m_iDashCharges;
}
