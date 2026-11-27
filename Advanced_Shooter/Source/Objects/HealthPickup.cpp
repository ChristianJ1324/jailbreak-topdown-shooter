/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : HealthPickup.cpp
Description : Health pickup behaviour
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#include "Objects/HealthPickup.h"
#include "Characters/Player.h"

HealthPickup::HealthPickup()
    : m_fHealAmount(35.f)
{
    SetSize(sf::Vector2f(24.f, 24.f));
}

void HealthPickup::Apply(Player& player)
{
    player.Heal(m_fHealAmount);
    SetActive(false);
}
