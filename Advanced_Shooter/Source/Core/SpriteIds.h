/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : SpriteIds.h
Description : Sprite asset identifiers
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#pragma once

namespace SpriteIds
{
    // sprite ids used across the game
    constexpr const char* k_szPlayerSprite = "player_sprite";
    constexpr const char* k_szEnemySprite = "enemy_sprite";
    constexpr const char* k_szPlayerBullet = "projectile_player_bullet";
    constexpr const char* k_szEnemyBullet = "projectile_enemy_bullet";
    constexpr const char* k_szMuzzleFlashEffect = "effect_muzzle_flash";
    constexpr const char* k_szHealthPickup = "health_pickup";
    constexpr const char* k_szCheckpoint = "checkpoint";
}
