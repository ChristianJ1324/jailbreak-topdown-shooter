/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : Wall.cpp
Description : Wall object setup
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#include "Objects/Wall.h"

Wall::Wall(const sf::Vector2f& vPosition)
{
    SetPosition(vPosition);
}

void Wall::Render(sf::RenderWindow& window)
{
    if (!IsActive())
    {
        return;
    }

    // draw using collision size
    sf::RectangleShape rectWall(GetSize());
    rectWall.setPosition(GetPosition());
    rectWall.setFillColor(sf::Color(116, 116, 116));
    rectWall.setOutlineColor(sf::Color(55, 55, 55));
    rectWall.setOutlineThickness(1.f);
    window.draw(rectWall);
}
