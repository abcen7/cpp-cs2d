#pragma once

#include "model/game_object.h"

class GameMap;

class Character : public GameObject {
public:
    ~Character() override;

    Character(const Character&) = delete;
    Character& operator=(const Character&) = delete;
    Character(Character&&) = delete;
    Character& operator=(Character&&) = delete;

    [[nodiscard]] int getHealth() const;
    [[nodiscard]] int getArmor() const;
    [[nodiscard]] float getSpeed() const;

    void setHealth(int health);
    void setArmor(int armor);
    void setSpeed(float speed);

    void applyDamage(int damageAmount);

    void tryMoveWithWallCollision(const GameMap& map, float deltaX, float deltaY);
    [[nodiscard]] bool overlapsBlockingTile(const GameMap& map) const;

    void updateModel(float deltaSeconds) override;

protected:
    Character(float positionX,
        float positionY,
        float width,
        float height,
        int health,
        int armor,
        float speed);

private:
    int m_health;
    int m_armor;
    float m_speed;
};
