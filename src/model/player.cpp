#include "model/player.h"

#include "model/bullet.h"
#include "model/game_map.h"
#include "model/game_state.h"
#include "model/pistol.h"
#include "model/rifle.h"
#include "model/shotgun.h"
#include "model/weapon.h"

#include "common/render_constants.h"

#include <cmath>

Player::Player(float spawnX, float spawnY)
    : Character(spawnX, spawnY, HITBOX_SIZE, HITBOX_SIZE, START_HEALTH, 0, MOVE_SPEED) {
    mp_weapons[0] = std::make_unique<Pistol>();
    mp_weapons[1] = std::make_unique<Rifle>();
    mp_weapons[2] = std::make_unique<Shotgun>();
}

Player::~Player() = default;

float Player::getAimAngleRadians() const {
    return m_aimAngleRadians;
}

void Player::setAimAngleRadians(float radians) {
    m_aimAngleRadians = radians;
}

void Player::setMovingForSpread(bool isMoving) {
    m_isMoving = isMoving;
}

void Player::updateWeapons(float deltaSeconds) {
    for (auto& pWeapon : mp_weapons) {
        if (pWeapon) {
            pWeapon->update(deltaSeconds);
        }
    }
}

Weapon* Player::getActiveWeapon() {
    if (m_activeWeaponSlot < 0 || m_activeWeaponSlot >= 3) {
        return nullptr;
    }
    return mp_weapons[static_cast<size_t>(m_activeWeaponSlot)].get();
}

const Weapon* Player::getActiveWeapon() const {
    if (m_activeWeaponSlot < 0 || m_activeWeaponSlot >= 3) {
        return nullptr;
    }
    return mp_weapons[static_cast<size_t>(m_activeWeaponSlot)].get();
}

int Player::getActiveWeaponSlot() const {
    return m_activeWeaponSlot;
}

int Player::getActiveWeaponAmmoInMagazine() const {
    const Weapon* pWeapon = getActiveWeapon();
    if (pWeapon == nullptr) {
        return 0;
    }
    return pWeapon->getAmmoInMagazine();
}

int Player::getActiveWeaponMagazineCapacity() const {
    const Weapon* pWeapon = getActiveWeapon();
    if (pWeapon == nullptr) {
        return 0;
    }
    return pWeapon->getMagazineCapacity();
}

bool Player::isActiveWeaponReloading() const {
    const Weapon* pWeapon = getActiveWeapon();
    if (pWeapon == nullptr) {
        return false;
    }
    return pWeapon->isReloading();
}

void Player::respawnAt(float worldX, float worldY) {
    setPositionX(worldX);
    setPositionY(worldY);
    setHealth(START_HEALTH);
    setArmor(0);
    m_isMoving = false;
    m_aimAngleRadians = 0.0f;
    m_activeWeaponSlot = 0;
    for (auto& pWeapon : mp_weapons) {
        if (pWeapon) {
            pWeapon->refillMagazineToFull();
        }
    }
}

void Player::switchWeaponSlot(int slotIndex) {
    if (slotIndex >= 0 && slotIndex < 3) {
        m_activeWeaponSlot = slotIndex;
    }
}

void Player::beginReloadActiveWeapon() {
    Weapon* pWeapon = getActiveWeapon();
    if (pWeapon == nullptr) {
        return;
    }
    pWeapon->beginReload();
}

void Player::tryFireWeapon(GameState& gameState, const GameMap& map) {
    Weapon* pWeapon = getActiveWeapon();
    if (pWeapon == nullptr) {
        return;
    }

    std::vector<PelletSpawn> pellets;
    if (!pWeapon->tryFire(getAimAngleRadians(), m_isMoving, pellets)) {
        return;
    }

    const float aim = getAimAngleRadians();
    float spawnX = 0.0f;
    float spawnY = 0.0f;
    characterMuzzleWorldPosition(aim, getPositionX(), getPositionY(), spawnX, spawnY);

    const float speed = pWeapon->getBulletSpeedPixelsPerSecond();
    const float weaponRange = pWeapon->getRangePixels();

    for (const auto& pellet : pellets) {
        const float dx = std::cos(pellet.angleRadians);
        const float dy = std::sin(pellet.angleRadians);
        gameState.addBullet(std::make_unique<Bullet>(
            spawnX, spawnY, dx, dy, speed, pellet.damage, weaponRange, &map, BulletTeam::Player));
    }
}

void Player::updateModel(float deltaSeconds) {
    Character::updateModel(deltaSeconds);
}
