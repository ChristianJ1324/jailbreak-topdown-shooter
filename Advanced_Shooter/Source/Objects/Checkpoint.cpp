/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : Checkpoint.cpp
Description : Checkpoint object behaviour
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#include "Objects/Checkpoint.h"
#include "Characters/Player.h"

Checkpoint::Checkpoint()
    : m_bIsActivated(false)
    , m_fAnimationTimer(0.f)
    , m_iAnimationFrame(0)
{
}

void Checkpoint::Update(float fDeltaTime)
{
    if (!m_bIsActivated || m_iAnimationFrame >= 5)
    {
        return;
    }

    // raise flag once
    m_fAnimationTimer += fDeltaTime;

    while (m_fAnimationTimer >= 0.08f &&
        m_iAnimationFrame < 5)
    {
        m_fAnimationTimer -= 0.08f;
        ++m_iAnimationFrame;
    }
}

void Checkpoint::Render(sf::RenderWindow& window)
{
    if (!IsActive() || !m_sprite.has_value())
    {
        return;
    }

    const sf::Texture& textureCheckpoint = m_sprite->getTexture();
    const sf::Vector2u vTextureSize = textureCheckpoint.getSize();

    if (vTextureSize.x == 0 || vTextureSize.y == 0)
    {
        return;
    }

    // horizontal flag strip
    const int iFrameWidth = static_cast<int>(vTextureSize.x) / 6;
    const int iFrameHeight = static_cast<int>(vTextureSize.y);

    if (iFrameWidth <= 0 || iFrameHeight <= 0)
    {
        return;
    }

    sf::Sprite spriteCheckpoint(textureCheckpoint);
    spriteCheckpoint.setTextureRect(sf::IntRect(
        sf::Vector2i(iFrameWidth * m_iAnimationFrame, 0),
        sf::Vector2i(iFrameWidth, iFrameHeight)));
    spriteCheckpoint.setOrigin(sf::Vector2f(
        static_cast<float>(iFrameWidth) * 0.5f,
        static_cast<float>(iFrameHeight)));
    spriteCheckpoint.setPosition(GetPosition() + sf::Vector2f(GetSize().x * 0.5f, GetSize().y));
    window.draw(spriteCheckpoint);
}

bool Checkpoint::Activate(Player& player)
{
    const bool bWasActivated = m_bIsActivated;

    // refresh respawn point
    if (!m_bIsActivated)
    {
        m_bIsActivated = true;
        m_fAnimationTimer = 0.f;
        m_iAnimationFrame = 0;
    }

    player.SetCheckpointPosition(GetPosition());
    return !bWasActivated;
}
