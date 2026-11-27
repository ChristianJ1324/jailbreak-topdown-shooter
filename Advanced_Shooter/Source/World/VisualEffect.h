/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : VisualEffect.h
Description : Short sprite effect animations
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#pragma once

#include <SFML/Graphics.hpp>

// tiny sprite strip muzzle flash
class VisualEffect
{
public:
    VisualEffect();

    void Initialise(
        const sf::Vector2f& vCenterPosition,
        const sf::Texture* pTexture,
        float fFrameTime,
        int iFrameCount,
        float fRotationRadians = 0.f);

    void Update(float fDeltaTime);
    void Render(sf::RenderWindow& window) const;

    bool IsActive() const;

private:
    const sf::Texture* m_pTexture;
    sf::Vector2f m_vCenterPosition;
    float m_fFrameTime;
    float m_fFrameTimer;
    float m_fRotationRadians;
    int m_iFrameCount;
    int m_iCurrentFrame;
    bool m_bIsActive;
};
