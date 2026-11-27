/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : SettingsManager.h
Description : Window settings loading
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#pragma once

#include <string>

// window and volume settings
class SettingsManager
{
public:
    SettingsManager();

    bool LoadFromFile(const std::string& strFilePath);
    bool SaveToFile(const std::string& strFilePath) const;

    unsigned int GetWindowWidth() const;
    unsigned int GetWindowHeight() const;
    bool GetVsyncEnabled() const;
    float GetMasterVolume() const;

    void SetMasterVolume(float fMasterVolume);

private:
    unsigned int m_uiWindowWidth;
    unsigned int m_uiWindowHeight;
    bool m_bVsyncEnabled;
    float m_fMasterVolume;
};
