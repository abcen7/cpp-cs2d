#pragma once

#include <mutex>

class GameMap;
class GameState;
class Player;

class InputController {
public:
    void reset(float viewWidth, float viewHeight);
    void setPlayer(Player* pPlayer);

    void handleKey(int key, bool isDown);
    void setMouseLocal(float localX, float localY);
    void setPrimaryFireHeld(bool held);

    void tick(
        Player& player,
        GameState& gameState,
        const GameMap& map,
        float deltaSeconds,
        float viewWidth,
        float viewHeight);

private:
    static void updateAimFromMouse(Player& player, float mouseLocalX, float mouseLocalY, float viewWidth, float viewHeight);

    bool m_keyW = false;
    bool m_keyA = false;
    bool m_keyS = false;
    bool m_keyD = false;

    float m_mouseLocalX = 0.0f;
    float m_mouseLocalY = 0.0f;

    bool m_primaryFireHeld = false;
    bool m_reloadRequested = false;
    int m_requestedWeaponSlot = -1;
    std::mutex m_inputMutex;
};
