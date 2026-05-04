#include "model/armor_pack.h"

#include "model/bot.h"
#include "model/player.h"

#include <algorithm>

namespace {

constexpr float BONUS_SIZE = 20.0f;
constexpr float BONUS_LIFETIME_SECONDS = 30.0f;
constexpr int ARMOR_BONUS = 50;
constexpr int MAX_ARMOR = 100;

}

ArmorPack::ArmorPack(float positionX, float positionY)
    : Bonus(positionX, positionY, BONUS_SIZE, BONUS_SIZE, BONUS_LIFETIME_SECONDS) {}

ArmorPack::~ArmorPack() = default;

BonusKind ArmorPack::getKind() const {
    return BonusKind::Armor;
}

void ArmorPack::onApplyPlayer(Player& rPlayer) {
    rPlayer.setArmor(std::min(MAX_ARMOR, rPlayer.getArmor() + ARMOR_BONUS));
}

void ArmorPack::onApplyBot(Bot& rBot) {
    rBot.setArmor(std::min(MAX_ARMOR, rBot.getArmor() + ARMOR_BONUS));
}
