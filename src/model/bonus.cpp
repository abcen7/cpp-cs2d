#include "model/bonus.h"

#include "model/bot.h"
#include "model/player.h"

Bonus::~Bonus() = default;

Bonus::Bonus(float positionX, float positionY, float width, float height, float lifetimeSeconds)
    : GameObject(positionX, positionY, width, height), m_timeRemainingSeconds(lifetimeSeconds) {}

bool Bonus::isActive() const {
    return m_isActive;
}

void Bonus::applyToPlayer(Player& rPlayer) {
    if (!m_isActive) {
        return;
    }
    onApplyPlayer(rPlayer);
    deactivate();
}

void Bonus::applyToBot(Bot& rBot) {
    if (!m_isActive) {
        return;
    }
    onApplyBot(rBot);
    deactivate();
}

void Bonus::updateModel(float deltaSeconds) {
    if (!m_isActive) {
        return;
    }
    m_timeRemainingSeconds -= deltaSeconds;
    if (m_timeRemainingSeconds <= 0.0f) {
        deactivate();
    }
}

void Bonus::deactivate() {
    m_isActive = false;
}
