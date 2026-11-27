/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : HealthPickup.h
Description : Health pickup behaviour
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#pragma once

#include "Objects/GameObject.h"

class Player;

// health pickup only
class HealthPickup : public GameObject
{
public:
    HealthPickup();

    void Apply(Player& player);

private:
    float m_fHealAmount;
};
