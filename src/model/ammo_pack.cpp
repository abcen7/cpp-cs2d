#include "model/ammo_pack.h"

#include "model/bot.h"
#include "model/player.h"
#include "model/weapon.h"

namespace {

constexpr float BONUS_SIZE = 20.0f;
constexpr float BONUS_LIFETIME_SECONDS = 28.0f;

}

AmmoPack::AmmoPack(float positionX, float positionY)
    : Bonus(positionX, positionY, BONUS_SIZE, BONUS_SIZE, BONUS_LIFETIME_SECONDS) {}

AmmoPack::~AmmoPack() = default;

BonusKind AmmoPack::getKind() const {
    return BonusKind::Ammo;
}

void AmmoPack::onApplyPlayer(Player& rPlayer) {
    if (Weapon* pWeapon = rPlayer.getActiveWeapon()) {
        pWeapon->refillMagazineToFull();
    }
}

void AmmoPack::onApplyBot(Bot& rBot) {
    if (Weapon* pWeapon = rBot.getActiveWeapon()) {
        pWeapon->refillMagazineToFull();
    }
}
