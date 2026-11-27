/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : ResourceManager.cpp
Description : Texture and font loading
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#include "Core/ResourceManager.h"
#include <utility>

bool ResourceManager::LoadTexture(const std::string& strId, const std::string& strFilePath)
{
    sf::Texture textureLoaded;

    if (!textureLoaded.loadFromFile(strFilePath))
    {
        return false;
    }

    m_mapTextures[strId] = std::move(textureLoaded);
    return true;
}

bool ResourceManager::LoadFont(const std::string& strId, const std::string& strFilePath)
{
    sf::Font fontLoaded;

    if (!fontLoaded.openFromFile(strFilePath))
    {
        return false;
    }

    m_mapFonts[strId] = std::move(fontLoaded);
    return true;
}

const sf::Texture* ResourceManager::GetTexture(const std::string& strId) const
{
    if (const auto itTexture = m_mapTextures.find(strId); itTexture != m_mapTextures.end())
    {
        return &(itTexture->second);
    }

    return nullptr;
}

const sf::Font* ResourceManager::GetFont(const std::string& strId) const
{
    if (const auto itFont = m_mapFonts.find(strId); itFont != m_mapFonts.end())
    {
        return &(itFont->second);
    }

    return nullptr;
}
