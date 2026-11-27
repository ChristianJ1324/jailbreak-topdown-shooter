/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : VectorMath.h
Description : Shared vector maths
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#pragma once

#include <SFML/System/Vector2.hpp>

namespace VectorMath
{
    // shared vector maths
    float Length(const sf::Vector2f& vValue);
    sf::Vector2f GetDirection(const sf::Vector2f& vValue);
    sf::Vector2f GetDirectionOrRight(const sf::Vector2f& vValue);
    float GetAngle(const sf::Vector2f& vDirection);
    bool IsZero(const sf::Vector2f& vValue);
}
