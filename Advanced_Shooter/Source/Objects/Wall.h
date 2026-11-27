/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : Wall.h
Description : Wall object setup
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#pragma once

#include "Objects/GameObject.h"

// simple square wall tile
class Wall : public GameObject
{
public:
    explicit Wall(const sf::Vector2f& vPosition);

    void Render(sf::RenderWindow& window) override;
};
