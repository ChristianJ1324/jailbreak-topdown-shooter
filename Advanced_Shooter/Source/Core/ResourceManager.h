/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : ResourceManager.h
Description : Texture and font loading
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>

// small texture and font cache
class ResourceManager
{
public:
    bool LoadTexture(const std::string& strId, const std::string& strFilePath);
    bool LoadFont(const std::string& strId, const std::string& strFilePath);

    const sf::Texture* GetTexture(const std::string& strId) const;
    const sf::Font* GetFont(const std::string& strId) const;

private:
    std::unordered_map<std::string, sf::Texture> m_mapTextures;
    std::unordered_map<std::string, sf::Font> m_mapFonts;
};
