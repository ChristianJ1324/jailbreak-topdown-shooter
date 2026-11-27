/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : Checkpoint.h
Description : Checkpoint object behaviour
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#pragma once

#include "Objects/GameObject.h"

class Player;

// checkpoint for respawn position
class Checkpoint : public GameObject
{
public:
    Checkpoint();

    void Update(float fDeltaTime);
    void Render(sf::RenderWindow& window) override;
    bool Activate(Player& player);

private:
    bool m_bIsActivated;
    float m_fAnimationTimer;
    int m_iAnimationFrame;
};
