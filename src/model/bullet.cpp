#include "model/bullet.h"

#include "model/game_map.h"

#include <cmath>

namespace {

constexpr float BULLET_SIZE = 3.0f;

}

Bullet::Bullet(float positionX,
    float positionY,
    float directionX,
    float directionY,
    float speedPixelsPerSecond,
    int damage,
    float maxRangePixels,
    const GameMap* pMap,
    BulletTeam team)
    : GameObject(positionX, positionY, BULLET_SIZE, BULLET_SIZE),
      m_directionX(directionX),
      m_directionY(directionY),
      m_speed(speedPixelsPerSecond),
      m_damage(damage),
      m_rangeRemaining(maxRangePixels),
      m_alive(true),
      mp_map(pMap),
      m_team(team) {
    const float length = std::sqrt(m_directionX * m_directionX + m_directionY * m_directionY);
    if (length > 1.0e-4f) {
        m_directionX /= length;
        m_directionY /= length;
    } else {
        m_directionX = 1.0f;
        m_directionY = 0.0f;
    }
}

void Bullet::updateModel(float deltaSeconds) {
    if (!m_alive || mp_map == nullptr) {
        return;
    }

    const float stepDistance = m_speed * deltaSeconds;
    if (stepDistance <= 0.0f) {
        return;
    }

    const float oldX = getPositionX();
    const float oldY = getPositionY();
    const float newX = oldX + m_directionX * stepDistance;
    const float newY = oldY + m_directionY * stepDistance;

    if (mp_map->isSegmentBlockedByWall(oldX, oldY, newX, newY)) {
        m_alive = false;
        return;
    }

    setPositionX(newX);
    setPositionY(newY);

    m_rangeRemaining -= stepDistance;
    if (m_rangeRemaining <= 0.0f) {
        m_alive = false;
    }
}

bool Bullet::isAlive() const {
    return m_alive;
}

int Bullet::getDamage() const {
    return m_damage;
}

BulletTeam Bullet::getTeam() const {
    return m_team;
}

void Bullet::destroy() {
    m_alive = false;
}
