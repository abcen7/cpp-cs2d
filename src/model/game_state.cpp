#include "model/game_state.h"

#include "model/ammo_pack.h"
#include "model/armor_pack.h"
#include "model/bot.h"
#include "model/bullet.h"
#include "model/game_map.h"
#include "model/health_pack.h"
#include "model/player.h"

#include <algorithm>
#include <cmath>
#include <random>
#include <utility>

namespace {

constexpr float BONUS_MIN_DISTANCE = 22.0f;
/// ~1.25 клетки; подбор в сеточном шутере — около центра бонуса, без капризного AABB.
constexpr float PICKUP_RADIUS = 40.0f;

bool canCharacterCollectBonus(float charX, float charY, float bonusX, float bonusY) {
    const float ddx = charX - bonusX;
    const float ddy = charY - bonusY;
    if (ddx * ddx + ddy * ddy <= PICKUP_RADIUS * PICKUP_RADIUS) {
        return true;
    }
    const int tpx = static_cast<int>(std::floor(charX / static_cast<float>(GameMap::TILE_SIZE)));
    const int tpy = static_cast<int>(std::floor(charY / static_cast<float>(GameMap::TILE_SIZE)));
    const int tbx = static_cast<int>(std::floor(bonusX / static_cast<float>(GameMap::TILE_SIZE)));
    const int tby = static_cast<int>(std::floor(bonusY / static_cast<float>(GameMap::TILE_SIZE)));
    return tpx == tbx && tpy == tby;
}

} // namespace

GameState::GameState() {
    m_randomEngine.seed(std::random_device{}());
}

GameState::~GameState() = default;

void GameState::setScreenState(GameScreenState state) {
    m_screenState = state;
}

GameScreenState GameState::getScreenState() const {
    return m_screenState;
}

bool GameState::beginNewGame(const std::string& mapPath) {
    m_playerScore = 0;
    m_bots.clear();
    m_bullets.clear();
    m_bonuses.clear();
    m_bonusSpawnCountdownSeconds = 0.0f;

    auto pMap = std::make_shared<GameMap>();
    if (!pMap->loadFromFile(mapPath)) {
        mp_map.reset();
        mp_player.reset();
        return false;
    }

    const auto spawn = pMap->getPlayerSpawnWorldPosition();
    if (!spawn.has_value()) {
        mp_map.reset();
        mp_player.reset();
        return false;
    }

    mp_map = std::move(pMap);
    mp_player = std::make_shared<Player>(spawn->first, spawn->second);

    const auto& botSpawns = mp_map->getBotSpawnWorldPositions();
    for (const auto& spawnPoint : botSpawns) {
        auto pBot = std::make_shared<Bot>(spawnPoint.first, spawnPoint.second, mp_map);
        pBot->setPatrolPoints(botSpawns);
        m_bots.push_back(std::move(pBot));
    }
    return true;
}

void GameState::clearSession() {
    m_playerScore = 0;
    m_bots.clear();
    m_bullets.clear();
    m_bonuses.clear();
    m_bonusSpawnCountdownSeconds = 0.0f;
    mp_map.reset();
    mp_player.reset();
}

std::shared_ptr<GameMap> GameState::getMap() const {
    return mp_map;
}

std::shared_ptr<Player> GameState::getPlayer() const {
    return mp_player;
}

void GameState::addBullet(std::unique_ptr<Bullet> bullet) {
    if (bullet) {
        m_bullets.push_back(std::move(bullet));
    }
}

void GameState::updateBullets(float deltaSeconds) {
    for (auto& pBullet : m_bullets) {
        if (pBullet) {
            pBullet->updateModel(deltaSeconds);
        }
    }

    processBulletCharacterCollisions();

    const auto removeIt = std::remove_if(
        m_bullets.begin(),
        m_bullets.end(),
        [](const std::unique_ptr<Bullet>& pBullet) { return !pBullet || !pBullet->isAlive(); });
    m_bullets.erase(removeIt, m_bullets.end());
}

const std::vector<std::unique_ptr<Bullet>>& GameState::getBullets() const {
    return m_bullets;
}

void GameState::updateBonuses(float deltaSeconds) {
    if (!mp_map) {
        return;
    }

    m_bonusSpawnCountdownSeconds -= deltaSeconds;
    if (m_bonusSpawnCountdownSeconds <= 0.0f) {
        trySpawnRandomBonus();
        std::uniform_real_distribution<float> interval(6.0f, 11.0f);
        m_bonusSpawnCountdownSeconds = interval(m_randomEngine);
    }

    processBonusCollisions();

    for (auto& pBonus : m_bonuses) {
        if (pBonus && pBonus->isActive()) {
            pBonus->updateModel(deltaSeconds);
        }
    }

    {
        const auto removeIt = std::remove_if(
            m_bonuses.begin(), m_bonuses.end(), [](const std::unique_ptr<Bonus>& pBonus) {
                return !pBonus || !pBonus->isActive();
            });
        m_bonuses.erase(removeIt, m_bonuses.end());
    }
}

const std::vector<std::unique_ptr<Bonus>>& GameState::getBonuses() const {
    return m_bonuses;
}

void GameState::processBonusCollisions() {
    for (auto& pBonus : m_bonuses) {
        if (!pBonus || !pBonus->isActive()) {
            continue;
        }

        const float bx = pBonus->getPositionX();
        const float by = pBonus->getPositionY();

        bool collected = false;
        if (mp_player) {
            if (canCharacterCollectBonus(mp_player->getPositionX(), mp_player->getPositionY(), bx, by)) {
                pBonus->applyToPlayer(*mp_player);
                collected = true;
            }
        }
        if (collected) {
            continue;
        }

        for (const auto& pBot : m_bots) {
            if (!pBot || pBot->getHealth() <= 0) {
                continue;
            }
            if (canCharacterCollectBonus(pBot->getPositionX(), pBot->getPositionY(), bx, by)) {
                pBonus->applyToBot(*pBot);
                break;
            }
        }
    }
}

bool GameState::isBonusSpotBlocked(float worldX, float worldY) const {
    for (const auto& pBonus : m_bonuses) {
        if (!pBonus || !pBonus->isActive()) {
            continue;
        }
        const float dx = pBonus->getPositionX() - worldX;
        const float dy = pBonus->getPositionY() - worldY;
        if (dx * dx + dy * dy < BONUS_MIN_DISTANCE * BONUS_MIN_DISTANCE) {
            return true;
        }
    }
    return false;
}

std::unique_ptr<Bonus> GameState::createRandomBonus(float worldX, float worldY) {
    std::uniform_int_distribution<int> kindDist(0, 2);
    const int k = kindDist(m_randomEngine);
    if (k == 0) {
        return std::make_unique<HealthPack>(worldX, worldY);
    }
    if (k == 1) {
        return std::make_unique<AmmoPack>(worldX, worldY);
    }
    return std::make_unique<ArmorPack>(worldX, worldY);
}

void GameState::trySpawnRandomBonus() {
    if (!mp_map) {
        return;
    }
    const auto& spots = mp_map->getBonusSpotWorldPositions();
    if (spots.empty()) {
        return;
    }

    std::vector<size_t> freeIndices;
    freeIndices.reserve(spots.size());
    for (size_t i = 0; i < spots.size(); ++i) {
        if (!isBonusSpotBlocked(spots[i].first, spots[i].second)) {
            freeIndices.push_back(i);
        }
    }
    if (freeIndices.empty()) {
        return;
    }

    std::uniform_int_distribution<size_t> pick(0, freeIndices.size() - 1);
    const auto& p = spots[freeIndices[pick(m_randomEngine)]];
    m_bonuses.push_back(createRandomBonus(p.first, p.second));
}

void GameState::updateBots(float deltaSeconds) {
    if (!mp_player || !mp_map) {
        return;
    }
    for (const auto& pBot : m_bots) {
        if (pBot && pBot->getHealth() > 0) {
            pBot->updateAi(*this, *mp_player, deltaSeconds);
        }
    }
}

const std::vector<std::shared_ptr<Bot>>& GameState::getBots() const {
    return m_bots;
}

int GameState::getPlayerScore() const {
    return m_playerScore;
}

void GameState::processBulletCharacterCollisions() {
    if (!mp_player) {
        return;
    }

    for (const auto& pBullet : m_bullets) {
        if (!pBullet || !pBullet->isAlive()) {
            continue;
        }

        auto hitCharacter = [&](auto& character) -> bool {
            const float halfW = character.getWidth() * 0.5f;
            const float halfH = character.getHeight() * 0.5f;
            const float left = character.getPositionX() - halfW;
            const float right = character.getPositionX() + halfW;
            const float top = character.getPositionY() - halfH;
            const float bottom = character.getPositionY() + halfH;
            const float x = pBullet->getPositionX();
            const float y = pBullet->getPositionY();
            if (x >= left && x <= right && y >= top && y <= bottom) {
                character.applyDamage(pBullet->getDamage());
                pBullet->destroy();
                return true;
            }
            return false;
        };

        if (pBullet->getTeam() == BulletTeam::Player) {
            for (const auto& pBot : m_bots) {
                if (!pBot || pBot->getHealth() <= 0) {
                    continue;
                }
                if (hitCharacter(*pBot)) {
                    if (pBot->getHealth() <= 0) {
                        m_playerScore += 1;
                    }
                    break;
                }
            }
        } else {
            if (mp_player->getHealth() > 0) {
                hitCharacter(*mp_player);
            }
        }
    }
}
