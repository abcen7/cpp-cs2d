#pragma once

#include "model/bullet.h"

#include <memory>
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
    GameState() = default;
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
    void updateBots(float deltaSeconds);
    [[nodiscard]] const std::vector<std::shared_ptr<Bot>>& getBots() const;
    [[nodiscard]] int getPlayerScore() const;

private:
    void processBulletCharacterCollisions();

    GameScreenState m_screenState = GameScreenState::Menu;
    std::shared_ptr<GameMap> mp_map;
    std::shared_ptr<Player> mp_player;
    std::vector<std::shared_ptr<Bot>> m_bots;
    std::vector<std::unique_ptr<Bullet>> m_bullets;
    int m_playerScore = 0;
};
