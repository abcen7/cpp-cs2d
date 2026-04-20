#pragma once

class Bot;
class GameMap;
class Player;

enum class BotAiState {
    Patrol,
    Chase,
    Attack
};

class BotStateMachine {
public:
    BotStateMachine() = default;

    void update(Bot& bot, const GameMap& map, const Player& player, float deltaSeconds);
    [[nodiscard]] BotAiState getCurrentState() const;

private:
    [[nodiscard]] bool canSeePlayer(const Bot& bot, const GameMap& map, const Player& player) const;
    [[nodiscard]] float distanceToPlayer(const Bot& bot, const Player& player) const;

    BotAiState m_state = BotAiState::Patrol;
    float m_lastSeenPlayerX = 0.0f;
    float m_lastSeenPlayerY = 0.0f;
};
