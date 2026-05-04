#include "model/bot_state_machine.h"

#include "model/bot.h"
#include "model/game_map.h"
#include "model/player.h"
#include "model/weapon.h"

#include <cmath>

namespace {

constexpr float DETECTION_RADIUS = 300.0f;
constexpr float CHASE_STOP_RADIUS = 26.0f;

}

void BotStateMachine::update(Bot& bot, const GameMap& map, const Player& player, float deltaSeconds) {
    (void)deltaSeconds;
    const float distance = distanceToPlayer(bot, player);
    const bool visible = canSeePlayer(bot, map, player);
    const Weapon* pWeapon = bot.getActiveWeapon();
    const float attackRange = (pWeapon != nullptr) ? pWeapon->getRangePixels() : 240.0f;

    if (visible) {
        m_lastSeenPlayerX = player.getPositionX();
        m_lastSeenPlayerY = player.getPositionY();
    }

    switch (m_state) {
        case BotAiState::Patrol:
            if (visible && distance <= DETECTION_RADIUS) {
                m_state = BotAiState::Chase;
            }
            break;
        case BotAiState::Chase:
            if (visible && distance <= attackRange) {
                m_state = BotAiState::Attack;
            } else if (!visible && std::hypot(bot.getPositionX() - m_lastSeenPlayerX, bot.getPositionY() - m_lastSeenPlayerY) <=
                                   CHASE_STOP_RADIUS) {
                m_state = BotAiState::Patrol;
            }
            break;
        case BotAiState::Attack:
            if (!visible || distance > attackRange + 20.0f) {
                m_state = BotAiState::Chase;
            }
            break;
    }
}

BotAiState BotStateMachine::getCurrentState() const {
    return m_state;
}

bool BotStateMachine::canSeePlayer(const Bot& bot, const GameMap& map, const Player& player) const {
    return !map.isSegmentBlockedByWall(bot.getPositionX(), bot.getPositionY(), player.getPositionX(), player.getPositionY());
}

float BotStateMachine::distanceToPlayer(const Bot& bot, const Player& player) const {
    return std::hypot(player.getPositionX() - bot.getPositionX(), player.getPositionY() - bot.getPositionY());
}
