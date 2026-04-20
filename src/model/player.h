#pragma once

#include <array>
#include <memory>

#include "model/character.h"

class GameMap;
class GameState;
class Weapon;

class Player : public Character {
public:
    explicit Player(float spawnX, float spawnY);
    ~Player() override;

    [[nodiscard]] float getAimAngleRadians() const;
    void setAimAngleRadians(float radians);

    void setMovingForSpread(bool isMoving);
    void updateWeapons(float deltaSeconds);

    void tryFireWeapon(GameState& gameState, const GameMap& map);
    void beginReloadActiveWeapon();
    void switchWeaponSlot(int slotIndex);

    [[nodiscard]] Weapon* getActiveWeapon();
    [[nodiscard]] const Weapon* getActiveWeapon() const;
    [[nodiscard]] int getActiveWeaponSlot() const;

    [[nodiscard]] int getActiveWeaponAmmoInMagazine() const;
    [[nodiscard]] int getActiveWeaponMagazineCapacity() const;
    [[nodiscard]] bool isActiveWeaponReloading() const;

    void updateModel(float deltaSeconds) override;

private:
    static constexpr float HITBOX_SIZE = 24.0f;
    static constexpr int START_HEALTH = 100;
    static constexpr float MOVE_SPEED = 200.0f;
    static constexpr float MUZZLE_OFFSET = 14.0f;

    float m_aimAngleRadians = 0.0f;
    bool m_isMoving = false;

    std::array<std::unique_ptr<Weapon>, 3> mp_weapons;
    int m_activeWeaponSlot = 0;
};
