/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : Game.cpp
Description : Main game loop and state control
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#include "Core/Game.h"
#include "Core/AudioIds.h"
#include "Core/SpriteIds.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace
{
    // text centering helper
    void SetTextCentered(sf::Text& textTarget, const sf::Vector2f& vCenter)
    {
        const sf::FloatRect rectBounds = textTarget.getLocalBounds();
        textTarget.setPosition(sf::Vector2f(
            vCenter.x - (rectBounds.size.x * 0.5f) - rectBounds.position.x,
            vCenter.y - (rectBounds.size.y * 0.5f) - rectBounds.position.y));
    }

    enum class MenuButtonAction
    {
        NewGame,
        LoadGame,
        OpenOptions,
        OpenControls,
        Resume,
        SaveGame,
        RestartLevel,
        RestartGame,
        ResumeCheckpoint,
        MainMenu,
        Back,
        QuitGame
    };

    // button data
    struct MenuButton
    {
        MenuButtonAction m_eAction;
        std::string m_strLabel;
        bool m_bEnabled;
    };

    // asset id and path
    struct AssetPath
    {
        const char* m_szId;
        const char* m_szPath;
    };

    std::vector<MenuButton> BuildMainMenuButtons()
    {
        // main menu buttons
        return {
            { MenuButtonAction::NewGame, "NEW GAME", true },
            { MenuButtonAction::LoadGame, "LOAD GAME", true },
            { MenuButtonAction::OpenOptions, "OPTIONS", true },
            { MenuButtonAction::OpenControls, "CONTROLS", true },
            { MenuButtonAction::QuitGame, "QUIT", true }
        };
    }

    std::vector<MenuButton> BuildPauseMenuButtons()
    {
        // pause menu buttons
        return {
            { MenuButtonAction::Resume, "RESUME", true },
            { MenuButtonAction::SaveGame, "SAVE GAME", true },
            { MenuButtonAction::OpenOptions, "OPTIONS", true },
            { MenuButtonAction::RestartLevel, "RESTART LEVEL", true },
            { MenuButtonAction::MainMenu, "MAIN MENU", true },
            { MenuButtonAction::QuitGame, "QUIT", true }
        };
    }

    std::vector<MenuButton> BuildGameOverButtons(bool bCanResumeFromCheckpoint)
    {
        std::vector<MenuButton> buttons;

        // checkpoint button if earned
        if (bCanResumeFromCheckpoint)
        {
            buttons.push_back({ MenuButtonAction::ResumeCheckpoint, "RESUME CHECKPOINT", true });
        }

        buttons.push_back({ MenuButtonAction::RestartGame, "RESTART RUN", true });
        buttons.push_back({ MenuButtonAction::MainMenu, "MAIN MENU", true });
        buttons.push_back({ MenuButtonAction::QuitGame, "QUIT", true });
        return buttons;
    }

    std::vector<MenuButton> BuildVictoryButtons()
    {
        return {
            { MenuButtonAction::RestartGame, "RESTART FROM LEVEL 1", true },
            { MenuButtonAction::MainMenu, "MAIN MENU", true }
        };
    }

    std::vector<MenuButton> BuildButtonsForState(GameState eState, bool bCanResumeFromCheckpoint)
    {
        // button list for current menu
        switch (eState)
        {
        case GameState::MainMenu:
            return BuildMainMenuButtons();

        case GameState::Paused:
            return BuildPauseMenuButtons();

        case GameState::OptionsMenu:
        case GameState::ControlsMenu:
            return { { MenuButtonAction::Back, "BACK", true } };

        case GameState::GameOver:
            return BuildGameOverButtons(bCanResumeFromCheckpoint);

        case GameState::Victory:
            return BuildVictoryButtons();

        case GameState::Playing:
            return {};
        }

        return {};
    }

    float GetMenuButtonTotalHeight(std::size_t uButtonCount)
    {
        // button stack height
        if (uButtonCount == 0)
        {
            return 0.f;
        }

        return (static_cast<float>(uButtonCount) * 48.f) +
            (static_cast<float>(uButtonCount - 1) * 14.f);
    }

    sf::FloatRect GetMenuButtonBounds(
        const sf::RenderWindow& window,
        std::size_t uButtonIndex,
        std::size_t uButtonCount)
    {
        // shared button bounds
        const float fWindowWidth = static_cast<float>(window.getSize().x);
        const float fWindowHeight = static_cast<float>(window.getSize().y);
        const float fTotalHeight = GetMenuButtonTotalHeight(uButtonCount);
        const float fStartY = ((fWindowHeight - fTotalHeight) * 0.5f) + 56.f;
        const float fButtonY = fStartY + (static_cast<float>(uButtonIndex) * 62.f);

        return sf::FloatRect(
            sf::Vector2f((fWindowWidth - 320.f) * 0.5f, fButtonY),
            sf::Vector2f(320.f, 48.f));
    }

    void DrawTextCentered(
        sf::RenderWindow& window,
        const sf::Font& font,
        const std::string& strText,
        unsigned int uiSize,
        const sf::Color& colour,
        const sf::Vector2f& vCenter)
    {
        // centred heading text
        sf::Text text(font);
        text.setString(strText);
        text.setCharacterSize(uiSize);
        text.setFillColor(colour);
        SetTextCentered(text, vCenter);
        window.draw(text);
    }

    void DrawMenuButton(
        sf::RenderWindow& window,
        const sf::Font& font,
        const MenuButton& button,
        const sf::FloatRect& rectBounds,
        bool bIsHovered)
    {
        // Menu button drawing
        sf::RectangleShape rectButton(rectBounds.size);
        rectButton.setPosition(rectBounds.position);

        if (!button.m_bEnabled)
        {
            rectButton.setFillColor(sf::Color(22, 24, 28, 210));
            rectButton.setOutlineColor(sf::Color(76, 82, 90));
        }
        else if (bIsHovered)
        {
            rectButton.setFillColor(sf::Color(46, 82, 104, 245));
            rectButton.setOutlineColor(sf::Color(180, 235, 255));
        }
        else
        {
            rectButton.setFillColor(sf::Color(18, 26, 38, 245));
            rectButton.setOutlineColor(sf::Color(120, 180, 230));
        }

        rectButton.setOutlineThickness(2.f);
        window.draw(rectButton);

        sf::Text textLabel(font);
        textLabel.setString(button.m_strLabel);
        textLabel.setCharacterSize(16);
        textLabel.setFillColor(button.m_bEnabled ? sf::Color::White : sf::Color(120, 126, 134));
        SetTextCentered(textLabel, rectBounds.position + (rectBounds.size * 0.5f));
        window.draw(textLabel);
    }

    void RenderMenuButtons(sf::RenderWindow& window, const sf::Font& font, const std::vector<MenuButton>& buttons)
    {
        // Menu mouse in screen space
        const sf::Vector2f vMousePosition = window.mapPixelToCoords(
            sf::Mouse::getPosition(window),
            window.getDefaultView());

        for (std::size_t uButtonIndex = 0; uButtonIndex < buttons.size(); ++uButtonIndex)
        {
            const sf::FloatRect rectBounds = GetMenuButtonBounds(window, uButtonIndex, buttons.size());
            const bool bIsHovered = buttons[uButtonIndex].m_bEnabled && rectBounds.contains(vMousePosition);
            DrawMenuButton(window, font, buttons[uButtonIndex], rectBounds, bIsHovered);
        }
    }

    std::optional<MenuButton> GetClickedMenuButton(
        const sf::RenderWindow& window,
        const std::vector<MenuButton>& buttons,
        const sf::Vector2f& vMousePosition)
    {
        // Clicked menu button
        for (std::size_t uButtonIndex = 0; uButtonIndex < buttons.size(); ++uButtonIndex)
        {
            const sf::FloatRect rectBounds = GetMenuButtonBounds(window, uButtonIndex, buttons.size());

            if (buttons[uButtonIndex].m_bEnabled && rectBounds.contains(vMousePosition))
            {
                return buttons[uButtonIndex];
            }
        }

        return std::nullopt;
    }

    void RenderMenuOverlay(
        sf::RenderWindow& window,
        const sf::Font& font,
        const std::string& strTitle,
        const std::string& strDetail,
        const std::vector<MenuButton>& buttons)
    {
        // shared menu overlay
        sf::RectangleShape rectOverlay;
        rectOverlay.setSize(sf::Vector2f(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)));
        rectOverlay.setPosition(sf::Vector2f(0.f, 0.f));
        rectOverlay.setFillColor(sf::Color(0, 0, 0, 165));
        window.draw(rectOverlay);

        const sf::FloatRect rectFirstButton = buttons.empty() ?
            sf::FloatRect(sf::Vector2f(0.f, static_cast<float>(window.getSize().y) * 0.5f), sf::Vector2f(0.f, 0.f)) :
            GetMenuButtonBounds(window, 0, buttons.size());
        const float fCenterX = static_cast<float>(window.getSize().x) * 0.5f;
        const float fTitleY = rectFirstButton.position.y - (strDetail.empty() ? 78.f : 106.f);

        DrawTextCentered(window, font, strTitle, 30, sf::Color(120, 230, 255), sf::Vector2f(fCenterX, fTitleY));

        if (!strDetail.empty())
        {
            DrawTextCentered(window, font, strDetail, 14, sf::Color(220, 230, 238), sf::Vector2f(fCenterX, rectFirstButton.position.y - 52.f));
        }

        RenderMenuButtons(window, font, buttons);
    }
}

Game::Game()
    : m_eCurrentState(GameState::MainMenu)
    , m_eMenuReturnState(GameState::MainMenu)
    , m_bCheckpointAvailable(false)
    , m_iCheckpointLevelIndex(-1)
    , m_vCheckpointPosition(0.f, 0.f)
    , m_bIsDraggingVolumeSlider(false)
    , m_fCurrentTimeScale(1.f)
    , m_fDamageFlashTimer(0.f)
    , m_fCameraShakeTimer(0.f)
    , m_fCameraShakeTime(0.f)
    , m_fCameraShakeStrength(0.f)
{
}

bool Game::Initialise()
{
    m_settingsManager.LoadFromFile("Data/Settings.ini");

    // load settings before creating window
    m_window.create(
        sf::VideoMode({ m_settingsManager.GetWindowWidth(), m_settingsManager.GetWindowHeight() }),
        "JAILBREAK");
    m_window.setVerticalSyncEnabled(m_settingsManager.GetVsyncEnabled());
    m_audioManager.SetMasterVolume(m_settingsManager.GetMasterVolume());

    if (!m_resourceManager.LoadFont("default", "Assets/Fonts/Pixeled.ttf"))
    {
        return false;
    }

    LoadAudioAssets();
    LoadSpriteTextures();

    m_player.SetSpriteTexture(m_resourceManager.GetTexture(SpriteIds::k_szPlayerSprite));
    m_hud.Initialise(m_resourceManager);
    m_levelManager.LoadLevel(0, m_resourceManager);
    LoadCurrentLevelToPlayer();
    UpdateMusicForState();
    return true;
}

void Game::Run()
{
    if (!Initialise())
    {
        return;
    }

    while (m_window.isOpen())
    {
        const float fDeltaTime = m_deltaClock.restart().asSeconds();
        ProcessInput();
        Update(fDeltaTime);
        Render();
    }
}

void Game::ProcessInput()
{
    while (const std::optional optEvent = m_window.pollEvent())
    {
        if (optEvent->is<sf::Event::Closed>())
        {
            m_window.close();
            return;
        }

        if (const auto* pKeyEvent = optEvent->getIf<sf::Event::KeyPressed>())
        {
            const sf::Keyboard::Key eKey = pKeyEvent->code;

            if (eKey == sf::Keyboard::Key::Escape || eKey == sf::Keyboard::Key::Tab)
            {
                // esc and tab toggle pause
                if (m_eCurrentState == GameState::Playing)
                {
                    ChangeState(GameState::Paused);
                }
                else if (m_eCurrentState == GameState::Paused)
                {
                    ChangeState(GameState::Playing);
                }

                continue;
            }

            if (eKey == sf::Keyboard::Key::F1)
            {
                m_debugWindow.Toggle();
                continue;
            }

            if (m_eCurrentState == GameState::Playing &&
                m_debugWindow.IsOpen())
            {
                m_debugWindow.HandleKey(eKey, m_player);
            }
        }

        if (const auto* pMouseEvent = optEvent->getIf<sf::Event::MouseButtonPressed>())
        {
            if (pMouseEvent->button == sf::Mouse::Button::Left)
            {
                const sf::Vector2f vMousePosition = m_window.mapPixelToCoords(
                    sf::Vector2i(pMouseEvent->position.x, pMouseEvent->position.y),
                    m_window.getDefaultView());

                if (m_eCurrentState == GameState::OptionsMenu && GetVolumeSliderBounds().contains(vMousePosition))
                {
                    // start volume slider drag
                    m_bIsDraggingVolumeSlider = true;
                    UpdateVolumeFromMouse(vMousePosition);
                    continue;
                }

                HandleMenuMouseClick(vMousePosition);
            }
        }

        if (const auto* pMouseMoveEvent = optEvent->getIf<sf::Event::MouseMoved>())
        {
            if (m_bIsDraggingVolumeSlider)
            {
                const sf::Vector2f vMousePosition = m_window.mapPixelToCoords(
                    sf::Vector2i(pMouseMoveEvent->position.x, pMouseMoveEvent->position.y),
                    m_window.getDefaultView());
                UpdateVolumeFromMouse(vMousePosition);
            }
        }

        if (const auto* pMouseReleaseEvent = optEvent->getIf<sf::Event::MouseButtonReleased>())
        {
            if (pMouseReleaseEvent->button == sf::Mouse::Button::Left && m_bIsDraggingVolumeSlider)
            {
                m_bIsDraggingVolumeSlider = false;
                m_settingsManager.SaveToFile("Data/Settings.ini");
            }
        }
    }

    if (m_eCurrentState == GameState::Playing)
    {
        // aim with world mouse position
        m_window.setView(CreateGameplayView());
        m_player.HandleInput(m_window);
        m_window.setView(m_window.getDefaultView());
    }
}

void Game::Update(float fDeltaTime)
{
    m_audioManager.Update();
    UpdateDamageFeedback(fDeltaTime);

    if (m_debugWindow.TakeSkipLevelRequest())
    {
        // handle debug requests here
        AdvanceLevel();
    }

    if (m_debugWindow.TakeReloadRequest())
    {
        ReloadCurrentLevel();
    }

    if (m_eCurrentState != GameState::Playing)
    {
        // reset slow motion in menus
        m_fCurrentTimeScale = 1.f;
        return;
    }

    UpdateTimeScale(fDeltaTime);
    const float fWorldDeltaTime = fDeltaTime * m_fCurrentTimeScale;

    // player updates at normal speed
    const float fPlayerHealthBefore = m_player.GetHealth();
    m_player.Update(fDeltaTime);
    Level& currentLevel = m_levelManager.GetCurrentLevel();
    currentLevel.Update(fWorldDeltaTime, m_player, m_debugWindow.ShouldFreezeEnemyAI());

    const int iPlayerShotCount = currentLevel.TakePlayerShotCount();
    if (iPlayerShotCount > 0)
    {
        TriggerShotFeedback();
        PlayRepeatedSound(AudioIds::k_szGunShoot, iPlayerShotCount);
    }

    const int iEnemyShotCount = currentLevel.TakeEnemyShotCount();

    if (iEnemyShotCount > 0)
    {
        m_audioManager.PlaySound(AudioIds::k_szGunShoot, 0.2f);
    }

    PlayRepeatedSound(AudioIds::k_szEnemyHurt, currentLevel.TakeEnemyHurtCount(), 0.85f);

    if (currentLevel.TakeCheckpointActivated())
    {
        // checkpoint trigger feedback
        m_bCheckpointAvailable = true;
        m_iCheckpointLevelIndex = m_levelManager.GetCurrentLevelIndex();
        m_vCheckpointPosition = m_player.GetCheckpointPosition();

        if (m_levelManager.GetCurrentLevelIndex() > 0)
        {
            m_hud.ShowCheckpointActivated();
        }
    }

    if (m_player.GetHealth() < fPlayerHealthBefore)
    {
        m_audioManager.PlaySound(AudioIds::k_szPlayerHurt);
        TriggerDamageFeedback();
    }

    m_hud.Update(m_player, m_levelManager.GetCurrentLevelIndex(), fDeltaTime);

    if (!m_player.IsAlive())
    {
        ChangeState(GameState::GameOver);
    }
    else if (currentLevel.IsCompleted())
    {
        AdvanceLevel();
    }
}

void Game::Render()
{
    m_window.clear(sf::Color(12, 17, 24));

    const bool bSubMenuOverMainMenu =
        (m_eCurrentState == GameState::OptionsMenu ||
            m_eCurrentState == GameState::ControlsMenu) &&
        m_eMenuReturnState == GameState::MainMenu;

    if (m_eCurrentState == GameState::MainMenu || bSubMenuOverMainMenu)
    {
        // main menu pages draw full screen
        if (m_eCurrentState == GameState::MainMenu)
        {
            RenderMainMenu();
        }
        else
        {
            if (m_eCurrentState == GameState::OptionsMenu)
            {
                RenderOptionsMenu();
            }
            else
            {
                RenderControlsMenu();
            }
        }

        m_window.display();
        return;
    }

    m_window.setView(CreateGameplayView());
    m_levelManager.GetCurrentLevel().Render(m_window);
    m_player.Render(m_window);

    if (m_debugWindow.ShouldShowCollisionBoxes())
    {
        RenderCollisionBoxes();
    }

    m_window.setView(m_window.getDefaultView());
    m_hud.Render(m_window);
    m_debugWindow.Render(m_window, m_resourceManager.GetFont("default"), m_player, m_levelManager);
    RenderDamageFlashOverlay();

    if (m_eCurrentState == GameState::Paused)
    {
        RenderPauseMenu();
    }
    else if (m_eCurrentState == GameState::OptionsMenu)
    {
        RenderOptionsMenu();
    }
    else if (m_eCurrentState == GameState::ControlsMenu)
    {
        RenderControlsMenu();
    }
    else if (m_eCurrentState == GameState::GameOver)
    {
        RenderGameOverMenu();
    }
    else if (m_eCurrentState == GameState::Victory)
    {
        RenderVictoryMenu();
    }

    m_window.display();
}

void Game::ChangeState(GameState eNewState)
{
    m_eCurrentState = eNewState;
    UpdateMusicForState();
}

void Game::LoadAudioAssets()
{
    // audio load table
    const std::array<AssetPath, 3> sounds = {
        AssetPath{ AudioIds::k_szGunShoot, "Assets/Audio/SFX/gun_shoot.wav" },
        AssetPath{ AudioIds::k_szEnemyHurt, "Assets/Audio/SFX/enemy_hurt.wav" },
        AssetPath{ AudioIds::k_szPlayerHurt, "Assets/Audio/SFX/player_hurt.wav" }
    };

    const std::array<AssetPath, 2> musicTracks = {
        AssetPath{ AudioIds::k_szMenuMusic, "Assets/Audio/Music/menu_music.ogg" },
        AssetPath{ AudioIds::k_szMainMusic, "Assets/Audio/Music/main_music.ogg" }
    };

    for (const AssetPath& sound : sounds)
    {
        m_audioManager.LoadSound(sound.m_szId, sound.m_szPath);
    }

    for (const AssetPath& musicTrack : musicTracks)
    {
        m_audioManager.LoadMusic(musicTrack.m_szId, musicTrack.m_szPath);
    }
}

void Game::LoadSpriteTextures()
{
    // sprite load table
    const std::array<AssetPath, 7> textures = {
        AssetPath{ SpriteIds::k_szPlayerSprite, "Assets/Sprites/Entities/player_sprite.png" },
        AssetPath{ SpriteIds::k_szEnemySprite, "Assets/Sprites/Entities/enemy_sprite.png" },
        AssetPath{ SpriteIds::k_szPlayerBullet, "Assets/Sprites/Projectiles/player_bullet.png" },
        AssetPath{ SpriteIds::k_szEnemyBullet, "Assets/Sprites/Projectiles/enemy_bullet.png" },
        AssetPath{ SpriteIds::k_szMuzzleFlashEffect, "Assets/Sprites/Projectiles/muzzle_flash.png" },
        AssetPath{ SpriteIds::k_szHealthPickup, "Assets/Sprites/Pickups/health_pickup.png" },
        AssetPath{ SpriteIds::k_szCheckpoint, "Assets/Sprites/Objects/checkpoint.png" }
    };

    for (const AssetPath& texture : textures)
    {
        m_resourceManager.LoadTexture(texture.m_szId, texture.m_szPath);
    }
}

void Game::StartNewGame()
{
    // start fresh run
    m_bCheckpointAvailable = false;
    m_iCheckpointLevelIndex = -1;
    m_vCheckpointPosition = sf::Vector2f(0.f, 0.f);
    m_levelManager.LoadLevel(0, m_resourceManager);
    LoadCurrentLevelToPlayer();
    ChangeState(GameState::Playing);
    m_fCurrentTimeScale = 1.f;
}

void Game::OpenOptionsMenu()
{
    m_eMenuReturnState = m_eCurrentState;
    ChangeState(GameState::OptionsMenu);
}

void Game::OpenControlsMenu()
{
    m_eMenuReturnState = m_eCurrentState;
    ChangeState(GameState::ControlsMenu);
}

void Game::ReturnToPreviousMenu()
{
    ChangeState(m_eMenuReturnState);
}

void Game::LoadSavedGame()
{
    SaveGameData saveData;

    if (!m_saveManager.LoadProgress("Data/SaveData.txt", saveData))
    {
        return;
    }

    saveData.m_iCurrentLevel = std::clamp(saveData.m_iCurrentLevel, 0, m_levelManager.GetLevelCount() - 1);
    m_levelManager.LoadLevel(saveData.m_iCurrentLevel, m_resourceManager);
    LoadCurrentLevelToPlayer();
    ApplySaveData(saveData);
    ChangeState(GameState::Playing);
    m_fCurrentTimeScale = 1.f;
}

void Game::SaveCurrentGame()
{
    m_saveManager.SaveProgress("Data/SaveData.txt", BuildSaveData());
}

void Game::LoadCurrentLevelToPlayer()
{
    const sf::Vector2f vSpawn = m_levelManager.GetCurrentLevel().GetPlayerSpawn();

    // reset player after level load
    m_player.SetPosition(vSpawn);
    m_player.SetCheckpointPosition(vSpawn);
    m_player.SetHealth(m_player.GetMaxHealth());
    m_player.RefreshDashCharges();
    m_player.SetBulletTimeMeter(1.f);
}

void Game::AdvanceLevel()
{
    // final level cleared
    const int iNextLevel = m_levelManager.GetCurrentLevelIndex() + 1;

    if (iNextLevel >= m_levelManager.GetLevelCount())
    {
        ChangeState(GameState::Victory);
        return;
    }

    m_levelManager.LoadLevel(iNextLevel, m_resourceManager);
    LoadCurrentLevelToPlayer();
    ChangeState(GameState::Playing);
    m_fCurrentTimeScale = 1.f;
}

void Game::ReloadCurrentLevel()
{
    m_levelManager.ReloadCurrentLevel(m_resourceManager);
    LoadCurrentLevelToPlayer();
    ChangeState(GameState::Playing);
}

void Game::ResumeFromCheckpoint()
{
    if (!CanResumeFromCheckpoint())
    {
        return;
    }

    // load the checkpoint level instead of the death level
    m_levelManager.LoadLevel(m_iCheckpointLevelIndex, m_resourceManager);
    LoadCurrentLevelToPlayer();
    m_bCheckpointAvailable = true;
    m_player.SetPosition(m_vCheckpointPosition);
    m_player.SetCheckpointPosition(m_vCheckpointPosition);
    m_player.SetBulletTimeMeter(1.f);
    m_fCurrentTimeScale = 1.f;
    ChangeState(GameState::Playing);
}

bool Game::CanResumeFromCheckpoint()
{
    return m_bCheckpointAvailable &&
        m_iCheckpointLevelIndex >= 0 &&
        m_iCheckpointLevelIndex < m_levelManager.GetLevelCount() &&
        m_levelManager.GetCurrentLevelIndex() > m_iCheckpointLevelIndex;
}

void Game::HandleMenuMouseClick(const sf::Vector2f& vMousePosition)
{
    const std::vector<MenuButton> buttons = BuildButtonsForState(m_eCurrentState, CanResumeFromCheckpoint());

    const std::optional<MenuButton> optButton = GetClickedMenuButton(m_window, buttons, vMousePosition);

    if (!optButton.has_value())
    {
        return;
    }

    // Menu button actions
    switch (optButton->m_eAction)
    {
    case MenuButtonAction::NewGame:
        StartNewGame();
        break;

    case MenuButtonAction::LoadGame:
        LoadSavedGame();
        break;

    case MenuButtonAction::OpenOptions:
        OpenOptionsMenu();
        break;

    case MenuButtonAction::OpenControls:
        OpenControlsMenu();
        break;

    case MenuButtonAction::Resume:
        ChangeState(GameState::Playing);
        break;

    case MenuButtonAction::SaveGame:
        SaveCurrentGame();
        break;

    case MenuButtonAction::RestartLevel:
        ReloadCurrentLevel();
        break;

    case MenuButtonAction::RestartGame:
        StartNewGame();
        break;

    case MenuButtonAction::ResumeCheckpoint:
        ResumeFromCheckpoint();
        break;

    case MenuButtonAction::MainMenu:
        ChangeState(GameState::MainMenu);
        break;

    case MenuButtonAction::Back:
        ReturnToPreviousMenu();
        break;

    case MenuButtonAction::QuitGame:
        m_window.close();
        break;
    }
}

void Game::RenderMainMenu()
{
    const sf::Font* pFont = m_resourceManager.GetFont("default");

    if (pFont == nullptr)
    {
        return;
    }

    DrawTextCentered(
        m_window,
        *pFont,
        "JAILBREAK",
        36,
        sf::Color(120, 230, 255),
        sf::Vector2f(
            static_cast<float>(m_window.getSize().x) * 0.5f,
            (static_cast<float>(m_window.getSize().y) * 0.5f) - 130.f));

    RenderMenuButtons(m_window, *pFont, BuildMainMenuButtons());
}

void Game::RenderPauseMenu()
{
    const sf::Font* pFont = m_resourceManager.GetFont("default");

    if (pFont == nullptr)
    {
        return;
    }

    RenderMenuOverlay(
        m_window,
        *pFont,
        "PAUSED",
        std::string("LEVEL ") + std::to_string(m_levelManager.GetCurrentLevelIndex() + 1),
        BuildPauseMenuButtons());
}

void Game::RenderOptionsMenu()
{
    const sf::Font* pFont = m_resourceManager.GetFont("default");

    if (pFont == nullptr)
    {
        return;
    }

    // simple options screen
    sf::RectangleShape rectOverlay;
    rectOverlay.setSize(sf::Vector2f(static_cast<float>(m_window.getSize().x), static_cast<float>(m_window.getSize().y)));
    rectOverlay.setPosition(sf::Vector2f(0.f, 0.f));
    rectOverlay.setFillColor(sf::Color(0, 0, 0, 165));
    m_window.draw(rectOverlay);

    const float fCenterX = static_cast<float>(m_window.getSize().x) * 0.5f;
    DrawTextCentered(m_window, *pFont, "OPTIONS", 30, sf::Color(120, 230, 255), sf::Vector2f(fCenterX, 146.f));
    DrawTextCentered(
        m_window,
        *pFont,
        "CLEAR EACH CELL BLOCK TO MOVE DEEPER INTO THE PRISON",
        13,
        sf::Color(220, 230, 238),
        sf::Vector2f(fCenterX, 210.f));
    DrawTextCentered(
        m_window,
        *pFont,
        "USE CHECKPOINTS, BULLET TIME, AND YOUR PISTOL TO SURVIVE",
        13,
        sf::Color(220, 230, 238),
        sf::Vector2f(fCenterX, 238.f));

    DrawTextCentered(
        m_window,
        *pFont,
        "MASTER VOLUME  " + std::to_string(static_cast<int>(std::round(m_settingsManager.GetMasterVolume()))),
        14,
        sf::Color::White,
        sf::Vector2f(fCenterX, 278.f));

    const sf::FloatRect rectSlider = GetVolumeSliderBounds();
    sf::RectangleShape rectTrack(rectSlider.size);
    rectTrack.setPosition(rectSlider.position);
    rectTrack.setFillColor(sf::Color(42, 48, 56));
    rectTrack.setOutlineColor(sf::Color(120, 180, 230));
    rectTrack.setOutlineThickness(2.f);
    m_window.draw(rectTrack);

    const float fVolumePercent = std::clamp(m_settingsManager.GetMasterVolume() / 100.f, 0.f, 1.f);
    sf::RectangleShape rectFill(sf::Vector2f(rectSlider.size.x * fVolumePercent, rectSlider.size.y));
    rectFill.setPosition(rectSlider.position);
    rectFill.setFillColor(sf::Color(120, 230, 255));
    m_window.draw(rectFill);

    sf::CircleShape circleHandle(10.f);
    circleHandle.setOrigin(sf::Vector2f(10.f, 10.f));
    circleHandle.setPosition(sf::Vector2f(rectSlider.position.x + (rectSlider.size.x * fVolumePercent), rectSlider.position.y + (rectSlider.size.y * 0.5f)));
    circleHandle.setFillColor(sf::Color::White);
    m_window.draw(circleHandle);

    RenderMenuButtons(m_window, *pFont, { { MenuButtonAction::Back, "BACK", true } });
}

void Game::RenderControlsMenu()
{
    const sf::Font* pFont = m_resourceManager.GetFont("default");

    if (pFont == nullptr)
    {
        return;
    }

    sf::RectangleShape rectOverlay;
    rectOverlay.setSize(sf::Vector2f(static_cast<float>(m_window.getSize().x), static_cast<float>(m_window.getSize().y)));
    rectOverlay.setPosition(sf::Vector2f(0.f, 0.f));
    rectOverlay.setFillColor(sf::Color(0, 0, 0, 165));
    m_window.draw(rectOverlay);

    const float fCenterX = static_cast<float>(m_window.getSize().x) * 0.5f;
    DrawTextCentered(m_window, *pFont, "CONTROLS", 30, sf::Color(120, 230, 255), sf::Vector2f(fCenterX, 112.f));

    // controls row data
    struct ControlRow
    {
        const char* m_szAction;
        const char* m_szBinding;
    };

    const std::array<ControlRow, 7> controls = {
        ControlRow{ "MOVE", "WASD OR ARROW KEYS" },
        ControlRow{ "AIM", "MOUSE" },
        ControlRow{ "FIRE", "LEFT MOUSE" },
        ControlRow{ "BULLET TIME", "HOLD RIGHT MOUSE" },
        ControlRow{ "DASH", "SPACE" },
        ControlRow{ "PAUSE", "ESC OR TAB" },
        ControlRow{ "MENUS", "CLICK BUTTONS" }
    };

    const float fActionRightX = fCenterX - 32.f;
    const float fBindingLeftX = fCenterX + 32.f;
    const float fFirstRowY = 162.f;
    const float fRowGap = 24.f;

    for (std::size_t uControlIndex = 0; uControlIndex < controls.size(); ++uControlIndex)
    {
        const float fRowY = fFirstRowY + (static_cast<float>(uControlIndex) * fRowGap);

        sf::Text textAction(*pFont);
        textAction.setString(controls[uControlIndex].m_szAction);
        textAction.setCharacterSize(13);
        textAction.setFillColor(sf::Color(190, 210, 220));
        const sf::FloatRect rectActionBounds = textAction.getLocalBounds();
        textAction.setPosition(sf::Vector2f(
            fActionRightX - rectActionBounds.size.x - rectActionBounds.position.x,
            fRowY - rectActionBounds.position.y));
        m_window.draw(textAction);

        sf::Text textBinding(*pFont);
        textBinding.setString(controls[uControlIndex].m_szBinding);
        textBinding.setCharacterSize(13);
        textBinding.setFillColor(sf::Color(245, 248, 255));
        const sf::FloatRect rectBindingBounds = textBinding.getLocalBounds();
        textBinding.setPosition(sf::Vector2f(
            fBindingLeftX - rectBindingBounds.position.x,
            fRowY - rectBindingBounds.position.y));
        m_window.draw(textBinding);
    }

    RenderMenuButtons(m_window, *pFont, { { MenuButtonAction::Back, "BACK", true } });
}

void Game::RenderGameOverMenu()
{
    const sf::Font* pFont = m_resourceManager.GetFont("default");

    if (pFont == nullptr)
    {
        return;
    }

    const bool bCanResumeFromCheckpoint = CanResumeFromCheckpoint();
    std::string strDetail = "RESTART THE RUN";

    if (bCanResumeFromCheckpoint)
    {
        strDetail = "CHECKPOINT AVAILABLE";
    }

    RenderMenuOverlay(
        m_window,
        *pFont,
        "GAME OVER",
        strDetail,
        BuildGameOverButtons(bCanResumeFromCheckpoint));
}

void Game::RenderVictoryMenu()
{
    const sf::Font* pFont = m_resourceManager.GetFont("default");

    if (pFont == nullptr)
    {
        return;
    }

    RenderMenuOverlay(
        m_window,
        *pFont,
        "YOU'VE ESCAPED THE PRISON",
        "THE RUN IS COMPLETE",
        BuildVictoryButtons());
}

void Game::RenderCollisionBoxes()
{
    sf::RectangleShape rectPlayerBox;
    rectPlayerBox.setSize(m_player.GetBounds().size);
    rectPlayerBox.setPosition(m_player.GetBounds().position);
    rectPlayerBox.setFillColor(sf::Color::Transparent);
    rectPlayerBox.setOutlineColor(sf::Color::Yellow);
    rectPlayerBox.setOutlineThickness(1.f);
    m_window.draw(rectPlayerBox);

    for (const Wall& wallCurrent : m_levelManager.GetCurrentLevel().GetWalls())
    {
        sf::RectangleShape rectWallBox;
        rectWallBox.setSize(wallCurrent.GetBounds().size);
        rectWallBox.setPosition(wallCurrent.GetBounds().position);
        rectWallBox.setFillColor(sf::Color::Transparent);
        rectWallBox.setOutlineColor(sf::Color(80, 160, 255));
        rectWallBox.setOutlineThickness(1.f);
        m_window.draw(rectWallBox);
    }
}

sf::View Game::CreateGameplayView() const
{
    const sf::Vector2u vWindowSize = m_window.getSize();
    const float fWindowWidth = static_cast<float>(vWindowSize.x);
    const float fWindowHeight = static_cast<float>(vWindowSize.y);

    sf::View viewGameplay(sf::FloatRect(
        sf::Vector2f(0.f, 0.f),
        sf::Vector2f(fWindowWidth, fWindowHeight)));

    viewGameplay.setViewport(sf::FloatRect(
        sf::Vector2f(0.f, 0.f),
        sf::Vector2f(1.f, 1.f)));

    // screenshake view offset
    if (m_fCameraShakeTimer > 0.f)
    {
        const float fShakePercent = m_fCameraShakeTimer / 0.16f;
        const float fOffsetX = std::sin(m_fCameraShakeTime * 76.f) * m_fCameraShakeStrength * fShakePercent;
        const float fOffsetY = std::cos(m_fCameraShakeTime * 91.f) * m_fCameraShakeStrength * fShakePercent;
        viewGameplay.setCenter(viewGameplay.getCenter() + sf::Vector2f(fOffsetX, fOffsetY));
    }

    return viewGameplay;
}

void Game::RenderDamageFlashOverlay()
{
    if (m_fDamageFlashTimer <= 0.f)
    {
        return;
    }

    const float fFlashPercent = std::clamp(m_fDamageFlashTimer / 0.22f, 0.f, 1.f);
    const sf::FloatRect rectScreen(
        sf::Vector2f(0.f, 0.f),
        sf::Vector2f(static_cast<float>(m_window.getSize().x), static_cast<float>(m_window.getSize().y)));

    sf::RectangleShape rectFlash(rectScreen.size);
    rectFlash.setPosition(rectScreen.position);
    rectFlash.setFillColor(sf::Color(220, 20, 35, static_cast<std::uint8_t>(95.f * fFlashPercent)));
    m_window.draw(rectFlash);
}

sf::FloatRect Game::GetVolumeSliderBounds() const
{
    const float fWindowWidth = static_cast<float>(m_window.getSize().x);
    return sf::FloatRect(
        sf::Vector2f((fWindowWidth - 360.f) * 0.5f, 312.f),
        sf::Vector2f(360.f, 12.f));
}

void Game::UpdateTimeScale(float fDeltaTime)
{
    const float fTargetTimeScale = m_player.WantsBulletTime() ? 0.1f : 1.f;
    const float fBlendAmount = std::min(1.f, 8.f * fDeltaTime);
    // smooth out the bullet time speed
    m_fCurrentTimeScale += (fTargetTimeScale - m_fCurrentTimeScale) * fBlendAmount;
}

void Game::UpdateDamageFeedback(float fDeltaTime)
{
    if (m_fDamageFlashTimer > 0.f)
    {
        m_fDamageFlashTimer = std::max(0.f, m_fDamageFlashTimer - fDeltaTime);
    }

    if (m_fCameraShakeTimer > 0.f)
    {
        m_fCameraShakeTimer = std::max(0.f, m_fCameraShakeTimer - fDeltaTime);
        m_fCameraShakeTime += fDeltaTime;

        if (m_fCameraShakeTimer <= 0.f)
        {
            m_fCameraShakeStrength = 0.f;
        }
    }
}

void Game::UpdateVolumeFromMouse(const sf::Vector2f& vMousePosition)
{
    const sf::FloatRect rectSlider = GetVolumeSliderBounds();
    const float fPercent = std::clamp(
        (vMousePosition.x - rectSlider.position.x) / rectSlider.size.x,
        0.f,
        1.f);
    m_settingsManager.SetMasterVolume(fPercent * 100.f);
    m_audioManager.SetMasterVolume(m_settingsManager.GetMasterVolume());
}

void Game::UpdateMusicForState()
{
    // music based game state
    const bool bUseMainMusic =
        m_eCurrentState == GameState::Playing ||
        m_eCurrentState == GameState::Paused ||
        ((m_eCurrentState == GameState::OptionsMenu ||
            m_eCurrentState == GameState::ControlsMenu) &&
            m_eMenuReturnState == GameState::Paused);

    m_audioManager.PlayMusic(bUseMainMusic ? AudioIds::k_szMainMusic : AudioIds::k_szMenuMusic);
}

void Game::PlayRepeatedSound(const std::string& strSoundId, int iCount, float fVolumeScale)
{
    // limit stacked sounds so our audio stays audible
    const int iClampedCount = std::clamp(iCount, 0, 4);

    for (int iSoundIndex = 0; iSoundIndex < iClampedCount; ++iSoundIndex)
    {
        m_audioManager.PlaySound(strSoundId, fVolumeScale);
    }
}

void Game::TriggerShotFeedback()
{
    StartCameraShake(0.06f, 1.8f);
}

void Game::StartCameraShake(float fDuration, float fStrength)
{
    if (fDuration <= 0.f || fStrength <= 0.f)
    {
        return;
    }

    if (m_fCameraShakeTimer <= 0.f || fStrength >= m_fCameraShakeStrength)
    {
        m_fCameraShakeStrength = fStrength;
    }

    m_fCameraShakeTimer = std::max(m_fCameraShakeTimer, fDuration);
    m_fCameraShakeTime = 0.f;
}

void Game::TriggerDamageFeedback()
{
    m_fDamageFlashTimer = 0.22f;
    StartCameraShake(0.16f, 5.f);
}

SaveGameData Game::BuildSaveData()
{
    SaveGameData saveData;

    // build save data
    saveData.m_iCurrentLevel = m_levelManager.GetCurrentLevelIndex();
    saveData.m_fPlayerHealth = m_player.GetHealth();
    saveData.m_vPlayerPosition = m_player.GetPosition();
    saveData.m_iCheckpointLevel = m_iCheckpointLevelIndex;
    saveData.m_vCheckpointPosition = m_vCheckpointPosition;
    saveData.m_iDashCharges = m_player.GetDashCharges();
    saveData.m_fBulletTimeMeter = m_player.GetBulletTimeMeter();
    return saveData;
}

void Game::ApplySaveData(const SaveGameData& saveData)
{
    m_player.SetPosition(saveData.m_vPlayerPosition);
    m_player.SetCheckpointPosition(saveData.m_vCheckpointPosition);
    m_player.SetHealth(saveData.m_fPlayerHealth);
    m_player.SetDashCharges(saveData.m_iDashCharges);
    m_player.SetBulletTimeMeter(saveData.m_fBulletTimeMeter);
    m_iCheckpointLevelIndex = saveData.m_iCheckpointLevel;
    m_vCheckpointPosition = saveData.m_vCheckpointPosition;
    m_bCheckpointAvailable = m_iCheckpointLevelIndex >= 0 &&
        m_iCheckpointLevelIndex < m_levelManager.GetLevelCount();
}
