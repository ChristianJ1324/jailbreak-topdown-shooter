/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : SaveGameData.h
Description : Save game data structures
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#pragma once

#include <SFML/System/Vector2.hpp>

// plain data for save files
struct SaveGameData
{
    int m_iCurrentLevel = 0;
    float m_fPlayerHealth = 100.f;
    sf::Vector2f m_vPlayerPosition = sf::Vector2f(0.f, 0.f);
    int m_iCheckpointLevel = -1;
    sf::Vector2f m_vCheckpointPosition = sf::Vector2f(0.f, 0.f);
    int m_iDashCharges = 3;
    float m_fBulletTimeMeter = 1.f;
};
