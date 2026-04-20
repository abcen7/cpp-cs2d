#include "model/rifle.h"

Rifle::Rifle() {
    m_ammoInMagazine = getMagazineCapacity();
}

int Rifle::getMagazineCapacity() const {
    return 30;
}

float Rifle::getFireIntervalSeconds() const {
    return 0.15f;
}

float Rifle::getReloadDurationSeconds() const {
    return 2.1f;
}

float Rifle::getRangePixels() const {
    return 900.0f;
}

float Rifle::getBulletSpeedPixelsPerSecond() const {
    return 950.0f;
}

int Rifle::getPelletCountPerShot() const {
    return 1;
}

int Rifle::getDamagePerPellet() const {
    return 25;
}

float Rifle::getBaseSpreadRadians() const {
    return 0.055f;
}

float Rifle::getMovingSpreadBonusRadians() const {
    return 0.06f;
}

float Rifle::getRecoilSpreadBonusRadians() const {
    return 0.012f;
}
