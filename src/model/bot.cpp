#include "model/bot.h"

#include "model/bullet.h"
#include "model/game_map.h"
#include "model/game_state.h"
#include "model/player.h"
#include "model/rifle.h"
#include "model/weapon.h"

#include "common/render_constants.h"

#include <cmath>

namespace {

constexpr float HITBOX_SIZE = 24.0f;
constexpr int START_HEALTH = 100;
constexpr float MOVE_SPEED = 165.0f;
constexpr float PATROL_REACH_RADIUS = 18.0f;

} // namespace

Bot::Bot(float spawnX, float spawnY, std::shared_ptr<GameMap> pMap)
    : Character(spawnX, spawnY, HITBOX_SIZE, HITBOX_SIZE, START_HEALTH, 0, MOVE_SPEED),
      mp_map(std::move(pMap)),
      mp_weapon(std::make_unique<Rifle>()) {}

Bot::~Bot() = default;

void Bot::setPatrolPoints(std::vector<std::pair<float, float>> patrolPoints) {
    m_patrolPoints = std::move(patrolPoints);
    m_patrolIndex = 0;
}

float Bot::getAimAngleRadians() const {
    return m_aimAngleRadians;
}

BotAiState Bot::getAiState() const {
    return m_stateMachine.getCurrentState();
}

Weapon* Bot::getActiveWeapon() {
    return mp_weapon.get();
}

const Weapon* Bot::getActiveWeapon() const {
    return mp_weapon.get();
}

void Bot::respawnAt(float worldX, float worldY) {
    setPositionX(worldX);
    setPositionY(worldY);
    setHealth(START_HEALTH);
    setArmor(0);
    m_aimAngleRadians = 0.0f;
    if (mp_weapon) {
        mp_weapon->refillMagazineToFull();
    }
}

void Bot::moveTowards(float targetX, float targetY, float deltaSeconds) {
    if (!mp_map) {
        return;
    }
    const float dx = targetX - getPositionX();
    const float dy = targetY - getPositionY();
    const float len = std::hypot(dx, dy);
    if (len < 1.0e-3f) {
        return;
    }
    const float nx = dx / len;
    const float ny = dy / len;
    m_aimAngleRadians = std::atan2(ny, nx);
    tryMoveWithWallCollision(*mp_map, nx * getSpeed() * deltaSeconds, ny * getSpeed() * deltaSeconds);
}

void Bot::fireAtPlayer(GameState& gameState) {
    if (!mp_weapon || !mp_map) {
        return;
    }
    std::vector<PelletSpawn> pellets;
    if (!mp_weapon->tryFire(m_aimAngleRadians, false, pellets)) {
        return;
    }

    float spawnX = 0.0f;
    float spawnY = 0.0f;
    characterMuzzleWorldPosition(m_aimAngleRadians, getPositionX(), getPositionY(), spawnX, spawnY);
    for (const auto& pellet : pellets) {
        gameState.addBullet(std::make_unique<Bullet>(
            spawnX,
            spawnY,
            std::cos(pellet.angleRadians),
            std::sin(pellet.angleRadians),
            mp_weapon->getBulletSpeedPixelsPerSecond(),
            pellet.damage,
            mp_weapon->getRangePixels(),
            mp_map.get(),
            BulletTeam::Bot));
    }
}

void Bot::updateAi(GameState& gameState, const Player& player, float deltaSeconds) {
    if (!mp_map || !mp_weapon) {
        return;
    }
    mp_weapon->update(deltaSeconds);
    m_stateMachine.update(*this, *mp_map, player, deltaSeconds);

    switch (m_stateMachine.getCurrentState()) {
        case BotAiState::Patrol: {
            if (m_patrolPoints.empty()) {
                break;
            }
            const auto [targetX, targetY] = m_patrolPoints[m_patrolIndex];
            moveTowards(targetX, targetY, deltaSeconds);
            if (std::hypot(targetX - getPositionX(), targetY - getPositionY()) <= PATROL_REACH_RADIUS) {
                m_patrolIndex = (m_patrolIndex + 1) % m_patrolPoints.size();
            }
            break;
        }
        case BotAiState::Chase:
            moveTowards(player.getPositionX(), player.getPositionY(), deltaSeconds);
            break;
        case BotAiState::Attack:
            m_aimAngleRadians = std::atan2(player.getPositionY() - getPositionY(), player.getPositionX() - getPositionX());
            fireAtPlayer(gameState);
            break;
    }
}

void Bot::updateModel(float deltaSeconds) {
    Character::updateModel(deltaSeconds);
}
