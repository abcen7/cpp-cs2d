#pragma once

#include "model/game_object.h"

class GameMap;

enum class BulletTeam {
    Player,
    Bot
};

class Bullet : public GameObject {
public:
    Bullet(float positionX,
        float positionY,
        float directionX,
        float directionY,
        float speedPixelsPerSecond,
        int damage,
        float maxRangePixels,
        const GameMap* pMap,
        BulletTeam team);

    void updateModel(float deltaSeconds) override;

    [[nodiscard]] bool isAlive() const;
    [[nodiscard]] int getDamage() const;
    [[nodiscard]] BulletTeam getTeam() const;
    void destroy();

private:
    float m_directionX;
    float m_directionY;
    float m_speed;
    int m_damage;
    float m_rangeRemaining;
    bool m_alive;
    const GameMap* mp_map;
    BulletTeam m_team;
};
