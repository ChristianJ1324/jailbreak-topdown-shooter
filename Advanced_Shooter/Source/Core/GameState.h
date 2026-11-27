/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : GameState.h
Description : Game state definitions
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#pragma once

// current game screen
enum class GameState
{
    MainMenu,
    Playing,
    Paused,
    OptionsMenu,
    ControlsMenu,
    GameOver,
    Victory
};
