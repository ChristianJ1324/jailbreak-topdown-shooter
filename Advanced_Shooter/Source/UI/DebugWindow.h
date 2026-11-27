/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : DebugWindow.h
Description : Debug overlay controls
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#pragma once

#include "Characters/Player.h"
#include "World/LevelManager.h"
#include <SFML/Graphics.hpp>

// debug overlay for testing
class DebugWindow
{
public:
    DebugWindow();

    void Toggle();
    void HandleKey(sf::Keyboard::Key eKey, Player& player);
    void Render(sf::RenderWindow& window, const sf::Font* pFont, const Player& player, const LevelManager& levelManager);

    bool IsOpen() const;
    bool ShouldShowCollisionBoxes() const;
    bool ShouldFreezeEnemyAI() const;
    bool TakeSkipLevelRequest();
    bool TakeReloadRequest();

private:
    bool m_bIsOpen;
    bool m_bShowCollisionBoxes;
    bool m_bFreezeEnemyAI;
    bool m_bSkipLevelRequested;
    bool m_bReloadRequested;
};
