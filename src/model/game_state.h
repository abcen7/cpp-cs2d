#pragma once

#include "model/bonus.h"
#include "model/bullet.h"

#include <memory>
#include <random>
#include <string>
#include <vector>

class Bot;
class GameMap;
class Player;

enum class GameScreenState {
    Menu,
    About,
    Playing,
    GameOver
};

class GameState {
public:
    GameState();
    ~GameState();

    void setScreenState(GameScreenState state);
    [[nodiscard]] GameScreenState getScreenState() const;

    [[nodiscard]] bool beginNewGame(const std::string& mapPath);
    void clearSession();

    [[nodiscard]] std::shared_ptr<GameMap> getMap() const;
    [[nodiscard]] std::shared_ptr<Player> getPlayer() const;

    void addBullet(std::unique_ptr<Bullet> bullet);
    void updateBullets(float deltaSeconds);
    [[nodiscard]] const std::vector<std::unique_ptr<Bullet>>& getBullets() const;
    void updateBonuses(float deltaSeconds);
    [[nodiscard]] const std::vector<std::unique_ptr<Bonus>>& getBonuses() const;
    void updateBots(float deltaSeconds);
    [[nodiscard]] const std::vector<std::shared_ptr<Bot>>& getBots() const;
    [[nodiscard]] int getPlayerScore() const;
    void updateMatchClock(float deltaSeconds);
    [[nodiscard]] bool hasReachedGameOverCondition() const;
    [[nodiscard]] float getElapsedMatchSeconds() const;
    [[nodiscard]] float getMatchDurationLimitSeconds() const;
    [[nodiscard]] int getScoreLimit() const;
    [[nodiscard]] bool isPlayerRespawnPending() const;
    [[nodiscard]] float getPlayerRespawnSecondsLeft() const;
    void updateRespawns(float deltaSeconds);

private:
    void processBulletCharacterCollisions();
    void processBonusCollisions();
    void trySpawnRandomBonus();
    [[nodiscard]] bool isBonusSpotBlocked(float worldX, float worldY) const;
    [[nodiscard]] std::unique_ptr<Bonus> createRandomBonus(float worldX, float worldY);
    void schedulePlayerRespawnIfNeeded();
    void scheduleBotRespawnIfNeeded(size_t botIndex);

    GameScreenState m_screenState = GameScreenState::Menu;
    std::shared_ptr<GameMap> mp_map;
    std::shared_ptr<Player> mp_player;
    std::vector<std::shared_ptr<Bot>> m_bots;
    std::vector<std::unique_ptr<Bullet>> m_bullets;
    std::vector<std::unique_ptr<Bonus>> m_bonuses;
    int m_playerScore = 0;
    float m_elapsedMatchSeconds = 0.0f;
    float m_matchDurationLimitSeconds = 180.0f;
    int m_scoreLimit = 15;
    float m_playerSpawnWorldX = 0.0f;
    float m_playerSpawnWorldY = 0.0f;
    bool m_playerRespawnPending = false;
    float m_playerRespawnTimerSeconds = 0.0f;
    std::vector<float> m_botRespawnTimerSeconds;
    static constexpr float PLAYER_RESPAWN_DELAY_SECONDS = 3.0f;
    static constexpr float BOT_RESPAWN_DELAY_SECONDS = 5.0f;
    float m_bonusSpawnCountdownSeconds = 0.0f;
    std::mt19937 m_randomEngine;
};
