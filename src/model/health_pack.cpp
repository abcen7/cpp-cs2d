#include "model/health_pack.h"

#include "model/bot.h"
#include "model/player.h"

#include <algorithm>

namespace {

constexpr float BONUS_SIZE = 20.0f;
constexpr float BONUS_LIFETIME_SECONDS = 32.0f;
constexpr int HEAL_AMOUNT = 25;
constexpr int MAX_STAT = 100;
ц
} // namespace

HealthPack::HealthPack(float positionX, float positionY)
    : Bonus(positionX, positionY, BONUS_SIZE, BONUS_SIZE, BONUS_LIFETIME_SECONDS) {}

HealthPack::~HealthPack() = default;

BonusKind HealthPack::getKind() const {
    return BonusKind::Health;
}

void HealthPack::onApplyPlayer(Player& rPlayer) {
    rPlayer.setHealth(std::min(MAX_STAT, rPlayer.getHealth() + HEAL_AMOUNT));
}

void HealthPack::onApplyBot(Bot& rBot) {
    rBot.setHealth(std::min(MAX_STAT, rBot.getHealth() + HEAL_AMOUNT));
}
