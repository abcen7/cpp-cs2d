#include "model/game_state.h"

#include "model/bullet.h"
#include "model/bot.h"
#include "model/game_map.h"
#include "model/player.h"

#include <algorithm>
#include <cmath>

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
