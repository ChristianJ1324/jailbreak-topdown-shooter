/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : Game.h
Description : Main game loop and state control
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#pragma once

#include "Characters/Player.h"
#include "Core/AudioManager.h"
#include "Core/GameState.h"
#include "Core/ResourceManager.h"
#include "Core/SaveManager.h"
#include "Core/SettingsManager.h"
#include "UI/DebugWindow.h"
#include "UI/Hud.h"
#include "World/LevelManager.h"
#include <SFML/Graphics.hpp>
#include <string>

// main game coordinator
class Game
{
public:
    Game();

    bool Initialise();
    void Run();

private:
    void ProcessInput();
    void Update(float fDeltaTime);
    void Render();
    void ChangeState(GameState eNewState);

    void LoadAudioAssets();
    void LoadSpriteTextures();
    void StartNewGame();
    void OpenOptionsMenu();
    void OpenControlsMenu();
    void ReturnToPreviousMenu();
    void LoadSavedGame();
    void SaveCurrentGame();
    void LoadCurrentLevelToPlayer();
    void AdvanceLevel();
    void ReloadCurrentLevel();
    void ResumeFromCheckpoint();
    bool CanResumeFromCheckpoint();
    void HandleMenuMouseClick(const sf::Vector2f& vMousePosition);
    void RenderMainMenu();
    void RenderPauseMenu();
    void RenderOptionsMenu();
    void RenderControlsMenu();
    void RenderGameOverMenu();
    void RenderVictoryMenu();
    void RenderCollisionBoxes();
    void RenderDamageFlashOverlay();
    sf::View CreateGameplayView() const;
    sf::FloatRect GetVolumeSliderBounds() const;
    void UpdateTimeScale(float fDeltaTime);
    void UpdateDamageFeedback(float fDeltaTime);
    void UpdateVolumeFromMouse(const sf::Vector2f& vMousePosition);
    void UpdateMusicForState();
    void PlayRepeatedSound(const std::string& strSoundId, int iCount, float fVolumeScale = 1.f);
    void TriggerShotFeedback();
    void StartCameraShake(float fDuration, float fStrength);
    void TriggerDamageFeedback();
    SaveGameData BuildSaveData();
    void ApplySaveData(const SaveGameData& saveData);

    sf::RenderWindow m_window;
    sf::Clock m_deltaClock;

    GameState m_eCurrentState;
    GameState m_eMenuReturnState;

    SettingsManager m_settingsManager;
    SaveManager m_saveManager;
    AudioManager m_audioManager;
    ResourceManager m_resourceManager;
    LevelManager m_levelManager;
    DebugWindow m_debugWindow;
    Hud m_hud;
    Player m_player;

    bool m_bCheckpointAvailable;
    int m_iCheckpointLevelIndex;
    sf::Vector2f m_vCheckpointPosition;
    bool m_bIsDraggingVolumeSlider;
    float m_fCurrentTimeScale;
    float m_fDamageFlashTimer;
    float m_fCameraShakeTimer;
    float m_fCameraShakeTime;
    float m_fCameraShakeStrength;
};
