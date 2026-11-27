/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : LevelManager.cpp
Description : Level file management
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#include "World/LevelManager.h"

LevelManager::LevelManager()
    : m_iCurrentLevelIndex(0)
    , m_levelFilePaths({
        "Data/Levels/Level1.txt",
        "Data/Levels/Level2.txt",
        "Data/Levels/Level3.txt",
        "Data/Levels/Level4.txt",
        "Data/Levels/Level5.txt"
    })
{
}

bool LevelManager::LoadLevel(int iLevelIndex, ResourceManager& resourceManager)
{
    // level index range check
    if (iLevelIndex < 0 || iLevelIndex >= static_cast<int>(m_levelFilePaths.size()))
    {
        return false;
    }

    m_iCurrentLevelIndex = iLevelIndex;
    return m_currentLevel.LoadFromFile(m_levelFilePaths[static_cast<std::size_t>(iLevelIndex)], resourceManager);
}

bool LevelManager::ReloadCurrentLevel(ResourceManager& resourceManager)
{
    // reload current level
    return LoadLevel(m_iCurrentLevelIndex, resourceManager);
}

int LevelManager::GetCurrentLevelIndex() const
{
    return m_iCurrentLevelIndex;
}

int LevelManager::GetLevelCount() const
{
    return static_cast<int>(m_levelFilePaths.size());
}

Level& LevelManager::GetCurrentLevel()
{
    return m_currentLevel;
}
