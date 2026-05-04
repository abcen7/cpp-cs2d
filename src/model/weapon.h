#pragma once

#include <vector>

struct PelletSpawn {
    float angleRadians = 0.0f;
    int damage = 0;
};

class Weapon {
public:
    virtual ~Weapon();

    Weapon(const Weapon&) = delete;
    Weapon& operator=(const Weapon&) = delete;
    Weapon(Weapon&&) = delete;
    Weapon& operator=(Weapon&&) = delete;

    [[nodiscard]] virtual int getMagazineCapacity() const = 0;
    [[nodiscard]] virtual float getFireIntervalSeconds() const = 0;
    [[nodiscard]] virtual float getReloadDurationSeconds() const = 0;
    [[nodiscard]] virtual float getRangePixels() const = 0;
    [[nodiscard]] virtual float getBulletSpeedPixelsPerSecond() const = 0;
    [[nodiscard]] virtual int getPelletCountPerShot() const = 0;
    [[nodiscard]] virtual int getDamagePerPellet() const = 0;
    [[nodiscard]] virtual float getBaseSpreadRadians() const = 0;
    [[nodiscard]] virtual float getMovingSpreadBonusRadians() const = 0;

    [[nodiscard]] int getAmmoInMagazine() const;
    [[nodiscard]] bool isReloading() const;
    [[nodiscard]] float getFireCooldownRemaining() const;

    void update(float deltaSeconds);

    void refillMagazineToFull();

    bool beginReload();

    /// Пытается выполнить выстрел и при успехе заполняет пеллеты углами в мировом пространстве.
    bool tryFire(float aimRadians, bool isMoving, std::vector<PelletSpawn>& outPellets);

protected:
    Weapon();

    [[nodiscard]] virtual float getRecoilSpreadBonusRadians() const;
    virtual void onShotFired();

    void decayRecoil(float deltaSeconds);

    int m_ammoInMagazine = 0;
    float m_fireCooldownRemaining = 0.0f;
    float m_reloadRemaining = 0.0f;
    float m_recoil = 0.0f;
};
