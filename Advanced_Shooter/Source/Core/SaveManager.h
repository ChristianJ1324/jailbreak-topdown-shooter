/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : SaveManager.h
Description : Save and load file handling
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#pragma once

#include "Core/SaveGameData.h"
#include <string>

// simple text save file
class SaveManager
{
public:
    bool SaveProgress(const std::string& strFilePath, const SaveGameData& saveData) const;
    bool LoadProgress(const std::string& strFilePath, SaveGameData& saveData) const;
};
