#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "model/character.h"
#include "model/bot_state_machine.h"

class GameMap;
class GameState;
class Player;
class Weapon;

class Bot : public Character {
public:
    Bot(float spawnX, float spawnY, std::shared_ptr<GameMap> pMap);
    ~Bot() override;

    void updateAi(GameState& gameState, const Player& player, float deltaSeconds);
    void setPatrolPoints(std::vector<std::pair<float, float>> patrolPoints);

    [[nodiscard]] float getAimAngleRadians() const;
    [[nodiscard]] BotAiState getAiState() const;
    [[nodiscard]] Weapon* getActiveWeapon();
    [[nodiscard]] const Weapon* getActiveWeapon() const;

    void updateModel(float deltaSeconds) override;

private:
    void moveTowards(float targetX, float targetY, float deltaSeconds);
    void fireAtPlayer(GameState& gameState);

    std::shared_ptr<GameMap> mp_map;
    std::unique_ptr<Weapon> mp_weapon;
    BotStateMachine m_stateMachine;
    std::vector<std::pair<float, float>> m_patrolPoints;
    size_t m_patrolIndex = 0;
    float m_aimAngleRadians = 0.0f;
};
