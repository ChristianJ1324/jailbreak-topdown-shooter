/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : Hud.h
Description : Player HUD drawing
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#pragma once

#include "Characters/Player.h"
#include "Core/ResourceManager.h"
#include <SFML/Graphics.hpp>
#include <string>

// player hud bars and popups
class Hud
{
public:
    Hud();

    bool Initialise(ResourceManager& resourceManager);
    void Update(const Player& player, int iCurrentLevel, float fDeltaTime);
    void Render(sf::RenderWindow& window);
    void ShowCheckpointActivated();

private:
    void DrawCenteredText(sf::RenderWindow& window, const std::string& strText, float fCenterX, float fY, unsigned int uiSize) const;
    void DrawTopCenterInfo(sf::RenderWindow& window) const;
    void DrawCheckpointMessage(sf::RenderWindow& window) const;
    void DrawHealthBar(sf::RenderWindow& window, const Player& player, const sf::Vector2f& vPosition) const;
    void DrawDashSegments(sf::RenderWindow& window, const Player& player, const sf::Vector2f& vPosition) const;
    void DrawBulletTimeBar(sf::RenderWindow& window, const Player& player, const sf::Vector2f& vPosition) const;

    const sf::Font* m_pFont;
    std::string m_strHealthText;
    std::string m_strLevelText;
    float m_fCheckpointMessageTimer;
    const Player* m_pPlayer;
};
