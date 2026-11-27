/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : DebugWindow.cpp
Description : Debug overlay controls
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#include "UI/DebugWindow.h"
#include <algorithm>

DebugWindow::DebugWindow()
    : m_bIsOpen(false)
    , m_bShowCollisionBoxes(false)
    , m_bFreezeEnemyAI(false)
    , m_bSkipLevelRequested(false)
    , m_bReloadRequested(false)
{
}

void DebugWindow::Toggle()
{
    m_bIsOpen = !m_bIsOpen;
}

void DebugWindow::HandleKey(sf::Keyboard::Key eKey, Player& player)
{
    if (!m_bIsOpen)
    {
        return;
    }

    // quick debug keys
    switch (eKey)
    {
    case sf::Keyboard::Key::Num1:
        player.SetInvincible(!player.IsInvincible());
        break;

    case sf::Keyboard::Key::Num2:
        player.SetHealth(player.GetMaxHealth());
        break;

    case sf::Keyboard::Key::Num3:
        player.SetMoveSpeed(player.GetMoveSpeed() + 25.f);
        break;

    case sf::Keyboard::Key::Num4:
        player.SetMoveSpeed(std::max(25.f, player.GetMoveSpeed() - 25.f));
        break;

    case sf::Keyboard::Key::Num6:
        m_bSkipLevelRequested = true;
        break;

    case sf::Keyboard::Key::Num7:
        m_bReloadRequested = true;
        break;

    case sf::Keyboard::Key::Num8:
        m_bShowCollisionBoxes = !m_bShowCollisionBoxes;
        break;

    case sf::Keyboard::Key::Num9:
        m_bFreezeEnemyAI = !m_bFreezeEnemyAI;
        break;

    default:
        break;
    }
}

void DebugWindow::Render(sf::RenderWindow& window, const sf::Font* pFont, const Player& player, const LevelManager& levelManager)
{
    if (!m_bIsOpen || pFont == nullptr)
    {
        return;
    }

    // debug text block
    std::string strText = "DEBUG WINDOW\n";
    strText += "LEVEL: " + std::to_string(levelManager.GetCurrentLevelIndex() + 1) + "\n";
    strText += "HEALTH: " + std::to_string(static_cast<int>(player.GetHealth())) + "\n";
    strText += "INVINCIBLE: " + std::string(player.IsInvincible() ? "ON" : "OFF") + "\n";
    strText += "ENEMY AI: " + std::string(m_bFreezeEnemyAI ? "FROZEN" : "ACTIVE") + "\n";
    strText += "COLLISION BOXES: " + std::string(m_bShowCollisionBoxes ? "ON" : "OFF") + "\n\n";
    strText += "DASH: " + std::to_string(player.GetDashCharges()) + "/" + std::to_string(player.GetMaxDashCharges()) + "\n\n";
    strText += "1 INVINCIBLE  2 HEAL  3/4 SPEED\n";
    strText += "6 SKIP  7 RELOAD\n";
    strText += "8 BOXES  9 FREEZE";

    sf::Text textDebug(*pFont);
    textDebug.setCharacterSize(12);
    textDebug.setFillColor(sf::Color::White);
    textDebug.setString(strText);
    textDebug.setPosition(sf::Vector2f(32.f, 64.f));

    const sf::FloatRect rectTextBounds = textDebug.getGlobalBounds();
    const float fPanelWidth = std::max(
        430.f,
        (rectTextBounds.position.x - 18.f) + rectTextBounds.size.x + 14.f);
    const float fPanelHeight =
        (rectTextBounds.position.y - 52.f) + rectTextBounds.size.y + 12.f;

    sf::RectangleShape rectPanel;
    rectPanel.setSize(sf::Vector2f(fPanelWidth, fPanelHeight));
    rectPanel.setPosition(sf::Vector2f(18.f, 52.f));
    rectPanel.setFillColor(sf::Color(0, 0, 0, 215));
    rectPanel.setOutlineColor(sf::Color(120, 230, 255));
    rectPanel.setOutlineThickness(2.f);
    window.draw(rectPanel);
    window.draw(textDebug);
}

bool DebugWindow::IsOpen() const
{
    return m_bIsOpen;
}

bool DebugWindow::ShouldShowCollisionBoxes() const
{
    return m_bShowCollisionBoxes;
}

bool DebugWindow::ShouldFreezeEnemyAI() const
{
    return m_bFreezeEnemyAI;
}

bool DebugWindow::TakeSkipLevelRequest()
{
    // clear skip request after reading
    const bool bValue = m_bSkipLevelRequested;
    m_bSkipLevelRequested = false;
    return bValue;
}

bool DebugWindow::TakeReloadRequest()
{
    // clear reload request after reading
    const bool bValue = m_bReloadRequested;
    m_bReloadRequested = false;
    return bValue;
}
