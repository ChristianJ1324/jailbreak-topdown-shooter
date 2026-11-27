/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2026] Media Design School
File Name : Level.cpp
Description : Level loading, spawning, and collisions
Author : Christian Jamieson
Mail : christian.jamieson@mds.ac.nz
**************************************************************************/
#include "World/Level.h"
#include "Core/SpriteIds.h"
#include "Core/VectorMath.h"
#include "World/CollisionManager.h"
#include <algorithm>
#include <fstream>

Level::Level()
    : m_vPlayerSpawn(64.f, 64.f)
    , m_iTileSize(32)
    , m_pResourceManager(nullptr)
    , m_iPlayerShotsToPlay(0)
    , m_iEnemyShotsToPlay(0)
    , m_iEnemyHurtSoundsToPlay(0)
    , m_fPlayerContactDamageTimer(0.f)
    , m_bIsCompleted(false)
    , m_bCheckpointActivated(false)
{
}

bool Level::LoadFromFile(const std::string& strFilePath, ResourceManager& resourceManager)
{
    m_pResourceManager = &resourceManager;
    Clear();

    // load our maps
    std::ifstream fileLevel(strFilePath);

    if (!fileLevel.is_open())
    {
        return false;
    }

    std::vector<std::string> levelRows;
    std::string strLine;

    while (std::getline(fileLevel, strLine))
    {
        levelRows.push_back(strLine);
    }

    for (std::size_t uRow = 0; uRow < levelRows.size(); ++uRow)
    {
        const std::string& strRow = levelRows[uRow];

        for (std::size_t uCol = 0; uCol < strRow.size(); ++uCol)
        {
            const sf::Vector2f vTilePosition(
                static_cast<float>(uCol * static_cast<std::size_t>(m_iTileSize)),
                static_cast<float>(uRow * static_cast<std::size_t>(m_iTileSize)));
            HandleTile(strRow[uCol], vTilePosition);
        }
    }

    return true;
}

void Level::Update(float fDeltaTime, Player& player, bool bFreezeEnemyAI)
{
    if (m_fPlayerContactDamageTimer > 0.f)
    {
        m_fPlayerContactDamageTimer -= fDeltaTime;
    }

    const sf::Vector2f vPlayerMuzzlePosition = player.GetMuzzlePosition();
    const std::size_t uProjectileCountBeforePlayerFire = m_projectiles.size();
    player.Fire(m_projectiles);

    if (m_projectiles.size() > uProjectileCountBeforePlayerFire)
    {
        // report player shot for sound and flash
        m_iPlayerShotsToPlay += static_cast<int>(m_projectiles.size() - uProjectileCountBeforePlayerFire);
        SpawnMuzzleFlash(
            vPlayerMuzzlePosition,
            player.GetAimDirection(),
            0.f);
    }

    // enemy shots spawn before bullet update
    for (std::unique_ptr<Enemy>& pEnemy : m_enemies)
    {
        if (!pEnemy || !pEnemy->IsAlive())
        {
            continue;
        }

        if (!bFreezeEnemyAI)
        {
            const std::size_t uProjectileCountBeforeEnemyUpdate = m_projectiles.size();
            pEnemy->UpdateAI(player, m_projectiles, CanEnemySeePlayer(*pEnemy, player));

            if (m_projectiles.size() > uProjectileCountBeforeEnemyUpdate)
            {
                m_iEnemyShotsToPlay += static_cast<int>(m_projectiles.size() - uProjectileCountBeforeEnemyUpdate);
                sf::Vector2f vMuzzleDirection = player.GetPosition() - pEnemy->GetPosition();

                // match enemy flash to bullet direction
                for (std::size_t uProjectileIndex = uProjectileCountBeforeEnemyUpdate;
                    uProjectileIndex < m_projectiles.size();
                    ++uProjectileIndex)
                {
                    if (m_projectiles[uProjectileIndex] &&
                        m_projectiles[uProjectileIndex]->GetOwner() == ProjectileOwner::Enemy)
                    {
                        vMuzzleDirection = m_projectiles[uProjectileIndex]->GetVelocity();
                        break;
                    }
                }

                SpawnMuzzleFlash(
                    pEnemy->GetPosition() + (pEnemy->GetSize() * 0.5f),
                    vMuzzleDirection,
                    32.f);
            }
        }
        else
        {
            pEnemy->SetVelocity(sf::Vector2f(0.f, 0.f));
        }

        pEnemy->Update(fDeltaTime);
        sf::Vector2f vEnemyVelocity = pEnemy->GetVelocity();
        CollisionManager::ResolveWallCollision(*pEnemy, m_walls, vEnemyVelocity);
        pEnemy->SetVelocity(vEnemyVelocity);
    }

    ResolveEnemyOverlaps();

    for (std::unique_ptr<Projectile>& pProjectile : m_projectiles)
    {
        if (pProjectile)
        {
            // add bullet sprite after spawn
            ApplyProjectileTexture(*pProjectile);
            pProjectile->Update(fDeltaTime);
        }
    }

    for (Checkpoint& checkpointCurrent : m_checkpoints)
    {
        checkpointCurrent.Update(fDeltaTime);
    }

    UpdateVisualEffects(fDeltaTime);
    CheckCollisions(player);
    RemoveInactiveObjects();
}

void Level::Render(sf::RenderWindow& window)
{
    for (Wall& wallCurrent : m_walls)
    {
        wallCurrent.Render(window);
    }

    for (Checkpoint& checkpointCurrent : m_checkpoints)
    {
        checkpointCurrent.Render(window);
    }

    for (HealthPickup& healthPickup : m_healthPickups)
    {
        if (healthPickup.IsActive())
        {
            healthPickup.Render(window);
        }
    }

    for (std::unique_ptr<Enemy>& pEnemy : m_enemies)
    {
        if (pEnemy)
        {
            pEnemy->Render(window);
        }
    }

    for (std::unique_ptr<Projectile>& pProjectile : m_projectiles)
    {
        if (pProjectile)
        {
            pProjectile->Render(window);
        }
    }

    RenderVisualEffects(window);
}

void Level::CheckCollisions(Player& player)
{
    // wall push before pickups and checkpoints
    sf::Vector2f vPlayerVelocity = player.GetVelocity();
    CollisionManager::ResolveWallCollision(player, m_walls, vPlayerVelocity);
    player.SetVelocity(vPlayerVelocity);

    for (HealthPickup& healthPickup : m_healthPickups)
    {
        if (healthPickup.IsActive() && CollisionManager::CheckCollision(player, healthPickup))
        {
            healthPickup.Apply(player);
        }
    }

    for (Checkpoint& checkpointCurrent : m_checkpoints)
    {
        if (CollisionManager::CheckCollision(player, checkpointCurrent))
        {
            if (checkpointCurrent.Activate(player))
            {
                m_bCheckpointActivated = true;
            }
        }
    }

    for (std::unique_ptr<Enemy>& pEnemy : m_enemies)
    {
        if (!pEnemy || !pEnemy->IsAlive())
        {
            continue;
        }

        if (CollisionManager::CheckCollision(player, *pEnemy) &&
            !player.IsInvincible() &&
            m_fPlayerContactDamageTimer <= 0.f)
        {
            // contact damage cooldown
            player.TakeDamage(8.f);
            m_fPlayerContactDamageTimer = 0.85f;
        }
    }

    const std::size_t uInitialProjectileCount = m_projectiles.size();

    // dont hit with bullets spawned this frame
    for (std::size_t uProjectileIndex = 0; uProjectileIndex < uInitialProjectileCount; ++uProjectileIndex)
    {
        Projectile* pProjectile = m_projectiles[uProjectileIndex].get();

        if (pProjectile == nullptr || !pProjectile->IsActive())
        {
            continue;
        }

        for (const Wall& wallCurrent : m_walls)
        {
            if (CollisionManager::CheckCollision(*pProjectile, wallCurrent))
            {
                pProjectile->SetActive(false);
                break;
            }
        }

        if (!pProjectile->IsActive())
        {
            continue;
        }

        if (pProjectile->GetOwner() == ProjectileOwner::Player)
        {
            // owner decides who bullet can hit
            for (std::unique_ptr<Enemy>& pEnemy : m_enemies)
            {
                if (pEnemy && pEnemy->IsAlive() && CollisionManager::CheckCollision(*pProjectile, *pEnemy))
                {
                    const bool bEnemyWasAlive = pEnemy->IsAlive();
                    const float fEnemyHealthBefore = pEnemy->GetHealth();
                    pEnemy->TakeDamage(pProjectile->GetDamage());

                    if (pEnemy->GetHealth() < fEnemyHealthBefore)
                    {
                        ++m_iEnemyHurtSoundsToPlay;
                    }

                    if (bEnemyWasAlive && !pEnemy->IsAlive())
                    {
                        // refill dash on kill
                        player.RefreshDashCharges();
                    }

                    pProjectile->SetActive(false);
                    break;
                }
            }
        }
        else if (CollisionManager::CheckCollision(*pProjectile, player) && !player.IsInvincible())
        {
            player.TakeDamage(pProjectile->GetDamage());
            pProjectile->SetActive(false);
        }
    }

    if (AreAllEnemiesDefeated())
    {
        m_bIsCompleted = true;
    }
}

void Level::ResetProjectiles()
{
    m_projectiles.clear();
    m_visualEffects.clear();
}

bool Level::IsCompleted() const
{
    return m_bIsCompleted;
}

sf::Vector2f Level::GetPlayerSpawn() const
{
    return m_vPlayerSpawn;
}

const std::vector<Wall>& Level::GetWalls() const
{
    return m_walls;
}

int Level::TakePlayerShotCount()
{
    const int iShotCount = m_iPlayerShotsToPlay;
    m_iPlayerShotsToPlay = 0;
    return iShotCount;
}

int Level::TakeEnemyShotCount()
{
    const int iShotCount = m_iEnemyShotsToPlay;
    m_iEnemyShotsToPlay = 0;
    return iShotCount;
}

int Level::TakeEnemyHurtCount()
{
    const int iHurtCount = m_iEnemyHurtSoundsToPlay;
    m_iEnemyHurtSoundsToPlay = 0;
    return iHurtCount;
}

bool Level::TakeCheckpointActivated()
{
    const bool bWasActivated = m_bCheckpointActivated;
    m_bCheckpointActivated = false;
    return bWasActivated;
}

void Level::Clear()
{
    m_walls.clear();
    m_checkpoints.clear();
    m_healthPickups.clear();
    m_enemies.clear();
    m_projectiles.clear();
    m_visualEffects.clear();
    m_vPlayerSpawn = sf::Vector2f(64.f, 64.f);
    m_iPlayerShotsToPlay = 0;
    m_iEnemyShotsToPlay = 0;
    m_iEnemyHurtSoundsToPlay = 0;
    m_fPlayerContactDamageTimer = 0.f;
    m_bIsCompleted = false;
    m_bCheckpointActivated = false;
}

void Level::HandleTile(char cTile, const sf::Vector2f& vTilePosition)
{
    // map letters # wall P spawn C close enemy T distance enemy H health K checkpoint
    switch (cTile)
    {
    case '#':
    {
        Wall wallNew(vTilePosition);

        m_walls.push_back(wallNew);
        break;
    }

    case 'P':
        m_vPlayerSpawn = vTilePosition;
        break;

    case 'C':
    {
        AddEnemy(std::make_unique<Enemy>(EnemyBehaviour::CloseRange), vTilePosition);
        break;
    }

    case 'T':
    {
        AddEnemy(std::make_unique<Enemy>(EnemyBehaviour::KeepDistance), vTilePosition);
        break;
    }

    case 'H':
    {
        HealthPickup healthPickup;
        healthPickup.SetPosition(vTilePosition + sf::Vector2f(4.f, 4.f));

        if (m_pResourceManager != nullptr)
        {
            healthPickup.SetTexture(m_pResourceManager->GetTexture(SpriteIds::k_szHealthPickup));
        }

        m_healthPickups.push_back(healthPickup);
        break;
    }

    case 'K':
    {
        Checkpoint checkpointNew;
        checkpointNew.SetPosition(vTilePosition);

        if (m_pResourceManager != nullptr)
        {
            checkpointNew.SetTexture(m_pResourceManager->GetTexture(SpriteIds::k_szCheckpoint));
        }

        m_checkpoints.push_back(checkpointNew);
        break;
    }

    default:
        break;
    }
}

void Level::AddEnemy(std::unique_ptr<Enemy> pEnemy, const sf::Vector2f& vPosition)
{
    if (!pEnemy)
    {
        return;
    }

    // spawn enemy from map tile
    pEnemy->SetPosition(vPosition);
    pEnemy->SetAttackDelay(static_cast<float>(m_enemies.size() % 5) * 0.35f);

    if (m_pResourceManager != nullptr)
    {
        pEnemy->SetSpriteTexture(m_pResourceManager->GetTexture(SpriteIds::k_szEnemySprite));
    }

    m_enemies.push_back(std::move(pEnemy));
}

void Level::ResolveEnemyOverlaps()
{
    // stop enemies stacking
    for (int iPass = 0; iPass < 4; ++iPass)
    {
        for (std::size_t uFirstIndex = 0; uFirstIndex < m_enemies.size(); ++uFirstIndex)
        {
            Enemy* pFirstEnemy = m_enemies[uFirstIndex].get();

            if (pFirstEnemy == nullptr || !pFirstEnemy->IsAlive())
            {
                continue;
            }

            for (std::size_t uSecondIndex = uFirstIndex + 1; uSecondIndex < m_enemies.size(); ++uSecondIndex)
            {
                Enemy* pSecondEnemy = m_enemies[uSecondIndex].get();

                if (pSecondEnemy == nullptr || !pSecondEnemy->IsAlive())
                {
                    continue;
                }

                const sf::FloatRect rectFirst = pFirstEnemy->GetBounds();
                const sf::FloatRect rectSecond = pSecondEnemy->GetBounds();
                const sf::Vector2f vFirstCenter = rectFirst.position + (rectFirst.size * 0.5f);
                const sf::Vector2f vSecondCenter = rectSecond.position + (rectSecond.size * 0.5f);
                sf::Vector2f vSeparationDirection = vSecondCenter - vFirstCenter;
                const float fDistance = VectorMath::Length(vSeparationDirection);

                if (fDistance >= 58.f)
                {
                    continue;
                }

                if (VectorMath::IsZero(vSeparationDirection))
                {
                    vSeparationDirection = (uFirstIndex % 2 == 0) ? sf::Vector2f(1.f, 0.f) : sf::Vector2f(0.f, 1.f);
                }
                else
                {
                    vSeparationDirection /= fDistance;
                }

                const sf::Vector2f vPush =
                    vSeparationDirection * (((58.f - fDistance) * 0.5f) + 0.1f);

                pFirstEnemy->MoveBy(sf::Vector2f(-vPush.x, -vPush.y));
                pSecondEnemy->MoveBy(vPush);
            }
        }
    }

    for (std::unique_ptr<Enemy>& pEnemy : m_enemies)
    {
        if (pEnemy == nullptr || !pEnemy->IsAlive())
        {
            continue;
        }

        sf::Vector2f vEnemyVelocity = pEnemy->GetVelocity();
        CollisionManager::ResolveWallCollision(*pEnemy, m_walls, vEnemyVelocity);
        pEnemy->SetVelocity(vEnemyVelocity);
    }
}

void Level::RemoveInactiveObjects()
{
    // remove inactive objects
    m_projectiles.erase(
        std::remove_if(
            m_projectiles.begin(),
            m_projectiles.end(),
            [](const std::unique_ptr<Projectile>& pProjectile)
            {
                return !pProjectile || !pProjectile->IsActive();
            }),
        m_projectiles.end());

    m_healthPickups.erase(
        std::remove_if(
            m_healthPickups.begin(),
            m_healthPickups.end(),
            [](const HealthPickup& healthPickup)
            {
                return !healthPickup.IsActive();
            }),
        m_healthPickups.end());
}

void Level::SpawnMuzzleFlash(
    const sf::Vector2f& vOwnerCenterPosition,
    const sf::Vector2f& vDirection,
    float fForwardOffsetFromCenter)
{
    const sf::Texture* pTexture = m_pResourceManager != nullptr ?
        m_pResourceManager->GetTexture(SpriteIds::k_szMuzzleFlashEffect) :
        nullptr;

    if (pTexture == nullptr)
    {
        return;
    }

    const sf::Vector2f vNormalDirection = VectorMath::GetDirectionOrRight(vDirection);
    const sf::Vector2f vMuzzlePosition =
        vOwnerCenterPosition +
        (vNormalDirection * fForwardOffsetFromCenter);

    VisualEffect effectNew;
    effectNew.Initialise(
        vMuzzlePosition,
        pTexture,
        0.035f,
        4,
        VectorMath::GetAngle(vNormalDirection));
    m_visualEffects.push_back(effectNew);
}

void Level::UpdateVisualEffects(float fDeltaTime)
{
    // add effect to list
    for (VisualEffect& effectCurrent : m_visualEffects)
    {
        effectCurrent.Update(fDeltaTime);
    }

    m_visualEffects.erase(
        std::remove_if(
            m_visualEffects.begin(),
            m_visualEffects.end(),
            [](const VisualEffect& effectCurrent)
            {
                return !effectCurrent.IsActive();
            }),
        m_visualEffects.end());
}

void Level::RenderVisualEffects(sf::RenderWindow& window)
{
    for (const VisualEffect& effectCurrent : m_visualEffects)
    {
        effectCurrent.Render(window);
    }
}

void Level::ApplyProjectileTexture(Projectile& projectileCurrent)
{
    // bullet sprite by owner
    if (m_pResourceManager == nullptr || projectileCurrent.HasTexture())
    {
        return;
    }

    if (projectileCurrent.GetOwner() == ProjectileOwner::Player)
    {
        projectileCurrent.SetTexture(m_pResourceManager->GetTexture(SpriteIds::k_szPlayerBullet));
        return;
    }

    if (projectileCurrent.GetOwner() == ProjectileOwner::Enemy)
    {
        projectileCurrent.SetTexture(m_pResourceManager->GetTexture(SpriteIds::k_szEnemyBullet));
    }
}

bool Level::CanEnemySeePlayer(const Enemy& enemy, const Player& player) const
{
    const sf::Vector2f vStart = enemy.GetPosition() + (enemy.GetSize() * 0.5f);
    const sf::Vector2f vEnd = player.GetPosition() + (player.GetSize() * 0.5f);
    const sf::Vector2f vLine = vEnd - vStart;
    const float fDistance = VectorMath::Length(vLine);

    if (fDistance <= 0.001f)
    {
        return true;
    }

    const int iStepCount = std::max(1, static_cast<int>(fDistance / 16.f));

    // sample between enemy and player
    for (int iStep = 1; iStep < iStepCount; ++iStep)
    {
        const sf::Vector2f vPoint =
            vStart + (vLine * (static_cast<float>(iStep) / static_cast<float>(iStepCount)));

        for (const Wall& wallCurrent : m_walls)
        {
            if (wallCurrent.IsActive() && wallCurrent.GetBounds().contains(vPoint))
            {
                return false;
            }
        }
    }

    return true;
}

bool Level::AreAllEnemiesDefeated() const
{
    if (m_enemies.empty())
    {
        // dont complete empty maps
        return false;
    }

    for (const std::unique_ptr<Enemy>& pEnemy : m_enemies)
    {
        if (pEnemy && pEnemy->IsAlive())
        {
            return false;
        }
    }

    return true;
}

