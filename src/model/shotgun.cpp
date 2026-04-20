#include "model/shotgun.h"

Shotgun::Shotgun() {
    m_ammoInMagazine = getMagazineCapacity();
}

int Shotgun::getMagazineCapacity() const {
    return 8;
}

float Shotgun::getFireIntervalSeconds() const {
    return 0.8f;
}

float Shotgun::getReloadDurationSeconds() const {
    return 2.6f;
}

float Shotgun::getRangePixels() const {
    return 220.0f;
}

float Shotgun::getBulletSpeedPixelsPerSecond() const {
    return 550.0f;
}

int Shotgun::getPelletCountPerShot() const {
    return 6;
}

int Shotgun::getDamagePerPellet() const {
    return 8;
}

float Shotgun::getBaseSpreadRadians() const {
    return 0.22f;
}

float Shotgun::getMovingSpreadBonusRadians() const {
    return 0.08f;
}
