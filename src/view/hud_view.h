#pragma once

#include <FL/Fl_Group.H>

class GameState;

class HudView : public Fl_Group {
public:
    HudView(int x, int y, int width, int height);

    void setGameState(GameState* pGameState);
    void clearGameState();

    void draw() override;

private:
    GameState* mp_gameState = nullptr;
};
