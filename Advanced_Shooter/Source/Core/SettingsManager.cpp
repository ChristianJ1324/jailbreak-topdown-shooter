/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : SettingsManager.cpp
Description : Window settings loading
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#include "Core/SettingsManager.h"
#include <algorithm>
#include <fstream>
#include <sstream>

SettingsManager::SettingsManager()
    : m_uiWindowWidth(1280)
    , m_uiWindowHeight(720)
    , m_bVsyncEnabled(true)
    , m_fMasterVolume(40.f)
{
}

bool SettingsManager::LoadFromFile(const std::string& strFilePath)
{
    std::ifstream fileSettings(strFilePath);

    if (!fileSettings.is_open())
    {
        return false;
    }

    std::string strLine;

    while (std::getline(fileSettings, strLine))
    {
        std::istringstream ssLine(strLine);
        std::string strKey;
        std::string strValue;

        if (!std::getline(ssLine, strKey, '=') || !std::getline(ssLine, strValue))
        {
            continue;
        }

        if (strKey == "WindowWidth")
        {
            m_uiWindowWidth = static_cast<unsigned int>(std::stoul(strValue));
        }
        else if (strKey == "WindowHeight")
        {
            m_uiWindowHeight = static_cast<unsigned int>(std::stoul(strValue));
        }
        else if (strKey == "Vsync")
        {
            m_bVsyncEnabled = strValue == "true";
        }
        else if (strKey == "MasterVolume")
        {
            SetMasterVolume(std::stof(strValue));
        }
    }

    return true;
}

bool SettingsManager::SaveToFile(const std::string& strFilePath) const
{
    // save all settings
    std::ofstream fileSettings(strFilePath);

    if (!fileSettings.is_open())
    {
        return false;
    }

    fileSettings << "WindowWidth=" << m_uiWindowWidth << '\n';
    fileSettings << "WindowHeight=" << m_uiWindowHeight << '\n';
    fileSettings << "Vsync=" << (m_bVsyncEnabled ? "true" : "false") << '\n';
    fileSettings << "MasterVolume=" << m_fMasterVolume << '\n';
    return true;
}

unsigned int SettingsManager::GetWindowWidth() const
{
    return m_uiWindowWidth;
}

unsigned int SettingsManager::GetWindowHeight() const
{
    return m_uiWindowHeight;
}

bool SettingsManager::GetVsyncEnabled() const
{
    return m_bVsyncEnabled;
}

float SettingsManager::GetMasterVolume() const
{
    return m_fMasterVolume;
}

void SettingsManager::SetMasterVolume(float fMasterVolume)
{
    m_fMasterVolume = std::clamp(fMasterVolume, 0.f, 100.f);
}
