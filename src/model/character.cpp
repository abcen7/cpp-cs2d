#include "model/character.h"

#include "model/game_map.h"

#include <algorithm>
#include <cmath>

namespace {

constexpr int MAX_HEALTH = 100;
constexpr int MAX_ARMOR = 100;

} // namespace

Character::~Character() = default;

Character::Character(
    float positionX, float positionY, float width, float height, int health, int armor, float speed)
    : GameObject(positionX, positionY, width, height), m_health(health), m_armor(armor), m_speed(speed) {}

int Character::getHealth() const {
    return m_health;
}

int Character::getArmor() const {
    return m_armor;
}

float Character::getSpeed() const {
    return m_speed;
}

void Character::setHealth(int health) {
    m_health = std::clamp(health, 0, MAX_HEALTH);
}

void Character::setArmor(int armor) {
    m_armor = std::clamp(armor, 0, MAX_ARMOR);
}

void Character::setSpeed(float speed) {
    m_speed = speed;
}

bool Character::overlapsBlockingTile(const GameMap& map) const {
    const float halfW = getWidth() * 0.5f;
    const float halfH = getHeight() * 0.5f;
    const float left = getPositionX() - halfW;
    const float right = getPositionX() + halfW;
    const float top = getPositionY() - halfH;
    const float bottom = getPositionY() + halfH;

    constexpr float epsilon = 0.01f;
    const int minGX = static_cast<int>(std::floor(left / static_cast<float>(GameMap::TILE_SIZE)));
    const int maxGX = static_cast<int>(std::floor((right - epsilon) / static_cast<float>(GameMap::TILE_SIZE)));
    const int minGY = static_cast<int>(std::floor(top / static_cast<float>(GameMap::TILE_SIZE)));
    const int maxGY = static_cast<int>(std::floor((bottom - epsilon) / static_cast<float>(GameMap::TILE_SIZE)));

    for (int gy = minGY; gy <= maxGY; ++gy) {
        for (int gx = minGX; gx <= maxGX; ++gx) {
            if (!map.isInsideMap(gx, gy)) {
                return true;
            }
            if (!map.getTile(gx, gy).isPassable()) {
                return true;
            }
        }
    }

    return false;
}

void Character::tryMoveWithWallCollision(const GameMap& map, float deltaX, float deltaY) {
    setPositionX(getPositionX() + deltaX);
    if (overlapsBlockingTile(map)) {
        setPositionX(getPositionX() - deltaX);
    }

    setPositionY(getPositionY() + deltaY);
    if (overlapsBlockingTile(map)) {
        setPositionY(getPositionY() - deltaY);
    }
}

void Character::applyDamage(int damageAmount) {
    if (damageAmount <= 0 || m_health <= 0) {
        return;
    }

    int hpDamage = damageAmount;
    if (m_armor > 0) {
        const int absorbed = std::min(m_armor, (damageAmount + 1) / 2);
        m_armor -= absorbed;
        hpDamage = damageAmount - absorbed;
    }

    m_health = std::max(0, m_health - hpDamage);
}

void Character::updateModel(float deltaSeconds) {
    (void)deltaSeconds;
}
