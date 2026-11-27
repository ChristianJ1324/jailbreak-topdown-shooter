/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : SaveManager.cpp
Description : Save and load file handling
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#include "Core/SaveManager.h"
#include <fstream>
#include <sstream>

bool SaveManager::SaveProgress(const std::string& strFilePath, const SaveGameData& saveData) const
{
    // save run progress
    std::ofstream fileSave(strFilePath);

    if (!fileSave.is_open())
    {
        return false;
    }

    fileSave << "CurrentLevel=" << saveData.m_iCurrentLevel << '\n';
    fileSave << "PlayerHealth=" << saveData.m_fPlayerHealth << '\n';
    fileSave << "PlayerX=" << saveData.m_vPlayerPosition.x << '\n';
    fileSave << "PlayerY=" << saveData.m_vPlayerPosition.y << '\n';
    fileSave << "CheckpointLevel=" << saveData.m_iCheckpointLevel << '\n';
    fileSave << "CheckpointX=" << saveData.m_vCheckpointPosition.x << '\n';
    fileSave << "CheckpointY=" << saveData.m_vCheckpointPosition.y << '\n';
    fileSave << "DashCharges=" << saveData.m_iDashCharges << '\n';
    fileSave << "BulletTimeMeter=" << saveData.m_fBulletTimeMeter << '\n';
    return true;
}

bool SaveManager::LoadProgress(const std::string& strFilePath, SaveGameData& saveData) const
{
    // read the save file
    std::ifstream fileSave(strFilePath);

    if (!fileSave.is_open())
    {
        return false;
    }

    std::string strLine;

    while (std::getline(fileSave, strLine))
    {
        std::istringstream ssLine(strLine);
        std::string strKey;
        std::string strValue;

        if (!std::getline(ssLine, strKey, '=') || !std::getline(ssLine, strValue))
        {
            continue;
        }

        if (strKey == "CurrentLevel")
        {
            saveData.m_iCurrentLevel = std::stoi(strValue);
        }
        else if (strKey == "PlayerHealth")
        {
            saveData.m_fPlayerHealth = std::stof(strValue);
        }
        else if (strKey == "PlayerX")
        {
            saveData.m_vPlayerPosition.x = std::stof(strValue);
        }
        else if (strKey == "PlayerY")
        {
            saveData.m_vPlayerPosition.y = std::stof(strValue);
        }
        else if (strKey == "CheckpointLevel")
        {
            saveData.m_iCheckpointLevel = std::stoi(strValue);
        }
        else if (strKey == "CheckpointX")
        {
            saveData.m_vCheckpointPosition.x = std::stof(strValue);
        }
        else if (strKey == "CheckpointY")
        {
            saveData.m_vCheckpointPosition.y = std::stof(strValue);
        }
        else if (strKey == "DashCharges")
        {
            saveData.m_iDashCharges = std::stoi(strValue);
        }
        else if (strKey == "BulletTimeMeter")
        {
            saveData.m_fBulletTimeMeter = std::stof(strValue);
        }
    }

    return true;
}
