#pragma once

#include "model/weapon.h"

class Pistol : public Weapon {
public:
    Pistol();

    [[nodiscard]] int getMagazineCapacity() const override;
    [[nodiscard]] float getFireIntervalSeconds() const override;
    [[nodiscard]] float getReloadDurationSeconds() const override;
    [[nodiscard]] float getRangePixels() const override;
    [[nodiscard]] float getBulletSpeedPixelsPerSecond() const override;
    [[nodiscard]] int getPelletCountPerShot() const override;
    [[nodiscard]] int getDamagePerPellet() const override;
    [[nodiscard]] float getBaseSpreadRadians() const override;
    [[nodiscard]] float getMovingSpreadBonusRadians() const override;
};
