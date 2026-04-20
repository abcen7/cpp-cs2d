#include "model/pistol.h"

Pistol::Pistol() {
    m_ammoInMagazine = getMagazineCapacity();
}

int Pistol::getMagazineCapacity() const {
    return 12;
}

float Pistol::getFireIntervalSeconds() const {
    return 0.4f;
}

float Pistol::getReloadDurationSeconds() const {
    return 1.6f;
}

float Pistol::getRangePixels() const {
    return 420.0f;
}

float Pistol::getBulletSpeedPixelsPerSecond() const {
    return 720.0f;
}

int Pistol::getPelletCountPerShot() const {
    return 1;
}

int Pistol::getDamagePerPellet() const {
    return 15;
}

float Pistol::getBaseSpreadRadians() const {
    return 0.05f;
}

float Pistol::getMovingSpreadBonusRadians() const {
    return 0.04f;
}
