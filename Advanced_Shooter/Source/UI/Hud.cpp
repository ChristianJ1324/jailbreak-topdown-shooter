/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : Hud.cpp
Description : Player HUD drawing
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#include "UI/Hud.h"
#include <algorithm>
#include <cmath>
#include <cstdint>

Hud::Hud()
    : m_pFont(nullptr)
    , m_strHealthText("HP: 100")
    , m_strLevelText("LEVEL 1")
    , m_fCheckpointMessageTimer(0.f)
    , m_pPlayer(nullptr)
{
}

bool Hud::Initialise(ResourceManager& resourceManager)
{
    m_pFont = resourceManager.GetFont("default");
    return m_pFont != nullptr;
}

void Hud::Update(const Player& player, int iCurrentLevel, float fDeltaTime)
{
    m_strHealthText = "HP: " + std::to_string(static_cast<int>(player.GetHealth()));
    m_strLevelText = "LEVEL " + std::to_string(iCurrentLevel + 1);
    m_pPlayer = &player;

    if (m_fCheckpointMessageTimer > 0.f)
    {
        m_fCheckpointMessageTimer = std::max(0.f, m_fCheckpointMessageTimer - fDeltaTime);
    }
}

void Hud::Render(sf::RenderWindow& window)
{
    if (m_pPlayer == nullptr)
    {
        return;
    }

    const float fWindowHeight = static_cast<float>(window.getSize().y);
    if (m_pFont == nullptr)
    {
        return;
    }

    // bottom centre hud
    const float fBarsX = (static_cast<float>(window.getSize().x) - 216.f) * 0.5f;
    const float fBarsY = (fWindowHeight - 32.f) - 8.f - 42.f;
    const sf::Vector2f vBarsPosition(fBarsX, fBarsY);
    const sf::FloatRect rectHudPanel(
        vBarsPosition - sf::Vector2f(6.f, 6.f),
        sf::Vector2f(
            216.f + 12.f,
            42.f + 12.f));

    sf::RectangleShape shapeHudPanel(rectHudPanel.size);
    shapeHudPanel.setPosition(rectHudPanel.position);
    shapeHudPanel.setFillColor(sf::Color(8, 12, 18, 205));
    shapeHudPanel.setOutlineColor(sf::Color(70, 88, 104, 210));
    shapeHudPanel.setOutlineThickness(1.f);
    window.draw(shapeHudPanel);

    DrawDashSegments(window, *m_pPlayer, vBarsPosition);
    DrawBulletTimeBar(window, *m_pPlayer, vBarsPosition + sf::Vector2f(0.f, 14.f));
    DrawHealthBar(
        window,
        *m_pPlayer,
        vBarsPosition + sf::Vector2f(0.f, 24.f));
    DrawTopCenterInfo(window);
    DrawCheckpointMessage(window);
}

void Hud::ShowCheckpointActivated()
{
    m_fCheckpointMessageTimer = 2.1f;
}

void Hud::DrawCenteredText(sf::RenderWindow& window, const std::string& strText, float fCenterX, float fY, unsigned int uiSize) const
{
    if (m_pFont == nullptr)
    {
        return;
    }

    sf::Text textHud(*m_pFont);
    textHud.setCharacterSize(uiSize);
    textHud.setFillColor(sf::Color::White);
    textHud.setString(strText);

    const sf::FloatRect rectText = textHud.getLocalBounds();
    textHud.setPosition(sf::Vector2f(
        fCenterX - (rectText.size.x * 0.5f) - rectText.position.x,
        fY - rectText.position.y));
    window.draw(textHud);
}

void Hud::DrawTopCenterInfo(sf::RenderWindow& window) const
{
    if (m_pFont == nullptr)
    {
        return;
    }

    const float fCenterX = static_cast<float>(window.getSize().x) * 0.5f;
    DrawCenteredText(window, m_strLevelText, fCenterX, 40.f, 14);
}

void Hud::DrawCheckpointMessage(sf::RenderWindow& window) const
{
    if (m_pFont == nullptr || m_fCheckpointMessageTimer <= 0.f)
    {
        return;
    }

    // blinking checkpoint popup
    const float fElapsed = 2.1f - m_fCheckpointMessageTimer;
    const float fFadeIn = std::clamp(fElapsed / 0.35f, 0.f, 1.f);
    const float fFadeOut = std::clamp(m_fCheckpointMessageTimer / 0.35f, 0.f, 1.f);
    const float fBlink = 0.55f + (0.45f * std::abs(std::sin(fElapsed * 4.5f)));
    const float fAlpha = std::clamp(fFadeIn * fFadeOut * fBlink, 0.f, 1.f);

    sf::Text textCheckpoint(*m_pFont);
    textCheckpoint.setCharacterSize(18);
    textCheckpoint.setFillColor(sf::Color(245, 245, 255, static_cast<std::uint8_t>(255.f * fAlpha)));
    textCheckpoint.setString("CHECKPOINT ACTIVATED");

    const sf::FloatRect rectText = textCheckpoint.getLocalBounds();
    textCheckpoint.setPosition(sf::Vector2f(
        (static_cast<float>(window.getSize().x) - rectText.size.x) * 0.5f - rectText.position.x,
        (static_cast<float>(window.getSize().y) - rectText.size.y) * 0.5f - rectText.position.y));
    window.draw(textCheckpoint);
}

void Hud::DrawHealthBar(sf::RenderWindow& window, const Player& player, const sf::Vector2f& vPosition) const
{
    // health text inside bar
    const float fHealthPercent = std::max(0.f, player.GetHealth() / player.GetMaxHealth());
    const sf::FloatRect rectBack(vPosition, sf::Vector2f(216.f, 18.f));
    const sf::FloatRect rectFill(vPosition, sf::Vector2f(216.f * fHealthPercent, 18.f));

    sf::RectangleShape shapeBack(rectBack.size);
    shapeBack.setPosition(rectBack.position);
    shapeBack.setFillColor(sf::Color(32, 12, 18));
    shapeBack.setOutlineColor(sf::Color(105, 54, 64));
    shapeBack.setOutlineThickness(1.f);
    window.draw(shapeBack);

    sf::RectangleShape shapeFill(rectFill.size);
    shapeFill.setPosition(rectFill.position);
    shapeFill.setFillColor(sf::Color(220, 60, 72));
    window.draw(shapeFill);

    if (m_pFont == nullptr)
    {
        return;
    }

    sf::Text textHealth(*m_pFont);
    textHealth.setCharacterSize(10);
    textHealth.setFillColor(sf::Color::White);
    textHealth.setString(m_strHealthText);

    const sf::FloatRect rectText = textHealth.getLocalBounds();
    textHealth.setPosition(sf::Vector2f(
        vPosition.x + ((216.f - rectText.size.x) * 0.5f) - rectText.position.x,
        vPosition.y + ((18.f - rectText.size.y) * 0.5f) - rectText.position.y));
    window.draw(textHealth);
}

void Hud::DrawDashSegments(sf::RenderWindow& window, const Player& player, const sf::Vector2f& vPosition) const
{
    const int iDashCharges = player.GetDashCharges();

    // dash charge blocks
    for (int iSegment = 0; iSegment < player.GetMaxDashCharges(); ++iSegment)
    {
        const float fSegmentX = vPosition.x + (static_cast<float>(iSegment) * 74.f);
        const bool bSegmentFilled = iSegment < iDashCharges;

        const sf::FloatRect rectSegment(
            sf::Vector2f(fSegmentX, vPosition.y),
            sf::Vector2f(68.f, 10.f));

        sf::RectangleShape shapeBack(rectSegment.size);
        shapeBack.setPosition(rectSegment.position);
        shapeBack.setFillColor(sf::Color(24, 28, 36));
        shapeBack.setOutlineColor(sf::Color(80, 90, 110));
        shapeBack.setOutlineThickness(1.f);
        window.draw(shapeBack);

        if (bSegmentFilled)
        {
            sf::RectangleShape shapeFill(rectSegment.size);
            shapeFill.setPosition(rectSegment.position);
            shapeFill.setFillColor(sf::Color(120, 210, 255));
            window.draw(shapeFill);
        }
    }
}

void Hud::DrawBulletTimeBar(sf::RenderWindow& window, const Player& player, const sf::Vector2f& vPosition) const
{
    // thin bullet time bar
    const float fMeterPercent = player.GetBulletTimeMeterPercent();
    const sf::FloatRect rectBack(vPosition, sf::Vector2f(216.f, 6.f));
    const sf::FloatRect rectFill(vPosition, sf::Vector2f(216.f * fMeterPercent, 6.f));

    sf::RectangleShape shapeBack(rectBack.size);
    shapeBack.setPosition(rectBack.position);
    shapeBack.setFillColor(sf::Color(13, 24, 34));
    shapeBack.setOutlineColor(sf::Color(55, 92, 120));
    shapeBack.setOutlineThickness(1.f);
    window.draw(shapeBack);

    sf::RectangleShape shapeFill(rectFill.size);
    shapeFill.setPosition(rectFill.position);
    shapeFill.setFillColor(sf::Color(95, 185, 235));
    window.draw(shapeFill);
}
