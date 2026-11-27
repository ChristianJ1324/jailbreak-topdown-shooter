/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : Level.h
Description : Level loading, spawning, and collisions
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#pragma once

#include "Characters/Enemy.h"
#include "Characters/Player.h"
#include "Core/ResourceManager.h"
#include "Objects/Checkpoint.h"
#include "Objects/HealthPickup.h"
#include "Objects/Wall.h"
#include "World/VisualEffect.h"
#include <memory>
#include <string>
#include <vector>

// current room objects and collisions
class Level
{
public:
    Level();

    bool LoadFromFile(const std::string& strFilePath, ResourceManager& resourceManager);

    void Update(float fDeltaTime, Player& player, bool bFreezeEnemyAI = false);
    void Render(sf::RenderWindow& window);

    void CheckCollisions(Player& player);
    void ResetProjectiles();

    bool IsCompleted() const;
    sf::Vector2f GetPlayerSpawn() const;
    int TakePlayerShotCount();
    int TakeEnemyShotCount();
    int TakeEnemyHurtCount();
    bool TakeCheckpointActivated();

    const std::vector<Wall>& GetWalls() const;

private:
    void Clear();
    void HandleTile(char cTile, const sf::Vector2f& vTilePosition);
    void AddEnemy(std::unique_ptr<Enemy> pEnemy, const sf::Vector2f& vPosition);
    void ResolveEnemyOverlaps();
    void RemoveInactiveObjects();
    void SpawnMuzzleFlash(
        const sf::Vector2f& vOwnerCenterPosition,
        const sf::Vector2f& vDirection,
        float fForwardOffsetFromCenter);
    void UpdateVisualEffects(float fDeltaTime);
    void RenderVisualEffects(sf::RenderWindow& window);
    void ApplyProjectileTexture(Projectile& projectileCurrent);
    bool CanEnemySeePlayer(const Enemy& enemy, const Player& player) const;
    bool AreAllEnemiesDefeated() const;

    std::vector<Wall> m_walls;
    std::vector<Checkpoint> m_checkpoints;
    std::vector<HealthPickup> m_healthPickups;
    std::vector<std::unique_ptr<Enemy>> m_enemies;
    std::vector<std::unique_ptr<Projectile>> m_projectiles;
    std::vector<VisualEffect> m_visualEffects;

    sf::Vector2f m_vPlayerSpawn;

    int m_iTileSize;
    const ResourceManager* m_pResourceManager;
    int m_iPlayerShotsToPlay;
    int m_iEnemyShotsToPlay;
    int m_iEnemyHurtSoundsToPlay;
    float m_fPlayerContactDamageTimer;
    bool m_bIsCompleted;
    bool m_bCheckpointActivated;
};
