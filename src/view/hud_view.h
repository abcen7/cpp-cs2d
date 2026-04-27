#pragma once

#include <FL/Fl_Group.H>
#include <mutex>

class GameState;

class HudView : public Fl_Group {
public:
    HudView(int x, int y, int width, int height);

    void setGameState(GameState* pGameState);
    void setStateMutex(std::mutex* pStateMutex);
    void clearGameState();

    void draw() override;

private:
    GameState* mp_gameState = nullptr;
    std::mutex* mp_stateMutex = nullptr;
};
