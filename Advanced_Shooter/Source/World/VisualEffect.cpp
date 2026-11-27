/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : VisualEffect.cpp
Description : Short sprite effect animations
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#include "World/VisualEffect.h"
#include <algorithm>

VisualEffect::VisualEffect()
    : m_pTexture(nullptr)
    , m_vCenterPosition(0.f, 0.f)
    , m_fFrameTime(0.05f)
    , m_fFrameTimer(0.f)
    , m_fRotationRadians(0.f)
    , m_iFrameCount(1)
    , m_iCurrentFrame(0)
    , m_bIsActive(false)
{
}

void VisualEffect::Initialise(
    const sf::Vector2f& vCenterPosition,
    const sf::Texture* pTexture,
    float fFrameTime,
    int iFrameCount,
    float fRotationRadians)
{
    // effect start values
    m_vCenterPosition = vCenterPosition;
    m_pTexture = pTexture;
    m_fFrameTime = std::max(0.01f, fFrameTime);
    m_iFrameCount = std::max(1, iFrameCount);
    m_fRotationRadians = fRotationRadians;
    m_fFrameTimer = 0.f;
    m_iCurrentFrame = 0;
    m_bIsActive = true;
}

void VisualEffect::Update(float fDeltaTime)
{
    if (!m_bIsActive)
    {
        return;
    }

    // turn off after final frame
    m_fFrameTimer += fDeltaTime;

    while (m_fFrameTimer >= m_fFrameTime)
    {
        m_fFrameTimer -= m_fFrameTime;
        ++m_iCurrentFrame;

        if (m_iCurrentFrame >= m_iFrameCount)
        {
            m_bIsActive = false;
            return;
        }
    }
}

void VisualEffect::Render(sf::RenderWindow& window) const
{
    if (!m_bIsActive)
    {
        return;
    }

    if (m_pTexture == nullptr)
    {
        return;
    }

    const sf::Vector2u vTextureSize = m_pTexture->getSize();

    if (vTextureSize.x == 0 || vTextureSize.y == 0)
    {
        return;
    }

    const int iFrameWidth = static_cast<int>(vTextureSize.x) / m_iFrameCount;

    if (iFrameWidth <= 0)
    {
        return;
    }

    // horizontal effect strip
    sf::Sprite spriteEffect(*m_pTexture);
    spriteEffect.setTextureRect(sf::IntRect(
        sf::Vector2i(iFrameWidth * m_iCurrentFrame, 0),
        sf::Vector2i(iFrameWidth, static_cast<int>(vTextureSize.y))));
    spriteEffect.setOrigin(sf::Vector2f(iFrameWidth * 0.5f, static_cast<float>(vTextureSize.y) * 0.5f));
    spriteEffect.setPosition(m_vCenterPosition);
    spriteEffect.setRotation(sf::radians(m_fRotationRadians));
    window.draw(spriteEffect);
}

bool VisualEffect::IsActive() const
{
    return m_bIsActive;
}
