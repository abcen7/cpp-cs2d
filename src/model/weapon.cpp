#include "model/weapon.h"

#include <algorithm>
#include <cmath>
#include <random>

namespace {

thread_local std::mt19937 g_rng{std::random_device{}()};

float randomUniform(float minValue, float maxValue) {
    std::uniform_real_distribution<float> dist(minValue, maxValue);
    return dist(g_rng);
}

} // namespace

Weapon::~Weapon() = default;

Weapon::Weapon() = default;

int Weapon::getAmmoInMagazine() const {
    return m_ammoInMagazine;
}

bool Weapon::isReloading() const {
    return m_reloadRemaining > 0.0f;
}

float Weapon::getFireCooldownRemaining() const {
    return m_fireCooldownRemaining;
}

float Weapon::getRecoilSpreadBonusRadians() const {
    return 0.0f;
}

void Weapon::onShotFired() {}

void Weapon::decayRecoil(float deltaSeconds) {
    m_recoil = std::max(0.0f, m_recoil - deltaSeconds * 3.0f);
}

void Weapon::update(float deltaSeconds) {
    if (m_reloadRemaining > 0.0f) {
        m_reloadRemaining -= deltaSeconds;
        if (m_reloadRemaining <= 0.0f) {
            m_reloadRemaining = 0.0f;
            m_ammoInMagazine = getMagazineCapacity();
        }
    }

    if (m_fireCooldownRemaining > 0.0f) {
        m_fireCooldownRemaining -= deltaSeconds;
        if (m_fireCooldownRemaining < 0.0f) {
            m_fireCooldownRemaining = 0.0f;
        }
    }

    decayRecoil(deltaSeconds);
}

bool Weapon::beginReload() {
    if (m_reloadRemaining > 0.0f) {
        return false;
    }
    if (m_ammoInMagazine >= getMagazineCapacity()) {
        return false;
    }
    m_reloadRemaining = getReloadDurationSeconds();
    return true;
}

bool Weapon::tryFire(float aimRadians, bool isMoving, std::vector<PelletSpawn>& outPellets) {
    outPellets.clear();

    if (m_reloadRemaining > 0.0f) {
        return false;
    }
    if (m_fireCooldownRemaining > 0.0f) {
        return false;
    }
    if (m_ammoInMagazine <= 0) {
        return false;
    }

    float spread = getBaseSpreadRadians();
    if (isMoving) {
        spread += getMovingSpreadBonusRadians();
    }
    spread += m_recoil * getRecoilSpreadBonusRadians();
    spread = std::max(spread, 0.0f);

    const int pellets = getPelletCountPerShot();
    const int damage = getDamagePerPellet();

    for (int i = 0; i < pellets; ++i) {
        const float offset = (spread > 0.0f) ? randomUniform(-spread, spread) : 0.0f;
        PelletSpawn pellet;
        pellet.angleRadians = aimRadians + offset;
        pellet.damage = damage;
        outPellets.push_back(pellet);
    }

    m_ammoInMagazine -= 1;
    m_fireCooldownRemaining = getFireIntervalSeconds();
    m_recoil += 1.0f;
    onShotFired();

    return true;
}
