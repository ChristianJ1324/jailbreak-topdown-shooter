/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : LevelManager.h
Description : Level file management
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#pragma once

#include "Core/ResourceManager.h"
#include "World/Level.h"
#include <string>
#include <vector>

// level file list lives here
class LevelManager
{
public:
    LevelManager();

    bool LoadLevel(int iLevelIndex, ResourceManager& resourceManager);
    bool ReloadCurrentLevel(ResourceManager& resourceManager);

    int GetCurrentLevelIndex() const;
    int GetLevelCount() const;
    Level& GetCurrentLevel();

private:
    int m_iCurrentLevelIndex;
    std::vector<std::string> m_levelFilePaths;
    Level m_currentLevel;
};
