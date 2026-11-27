/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : VectorMath.cpp
Description : Shared vector maths
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#include "Core/VectorMath.h"
#include <cmath>

float VectorMath::Length(const sf::Vector2f& vValue)
{
    return std::sqrt((vValue.x * vValue.x) + (vValue.y * vValue.y));
}

sf::Vector2f VectorMath::GetDirection(const sf::Vector2f& vValue)
{
    const float fLength = Length(vValue);

    if (fLength <= 0.001f)
    {
        return sf::Vector2f(0.f, 0.f);
    }

    return vValue / fLength;
}

sf::Vector2f VectorMath::GetDirectionOrRight(const sf::Vector2f& vValue)
{
    const sf::Vector2f vDirection = GetDirection(vValue);
    return IsZero(vDirection) ? sf::Vector2f(1.f, 0.f) : vDirection;
}

float VectorMath::GetAngle(const sf::Vector2f& vDirection)
{
    if (IsZero(vDirection))
    {
        return 0.f;
    }

    return std::atan2(vDirection.y, vDirection.x);
}

bool VectorMath::IsZero(const sf::Vector2f& vValue)
{
    return std::abs(vValue.x) <= 0.001f &&
        std::abs(vValue.y) <= 0.001f;
}
