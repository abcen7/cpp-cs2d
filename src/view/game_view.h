#pragma once

#include <FL/Fl_Group.H>
#include <mutex>

class GameState;
class InputController;

class GameView : public Fl_Group {
public:
    GameView(int x, int y, int width, int height);

    void setBindings(GameState* pGameState, InputController* pInputController);
    void setStateMutex(std::mutex* pStateMutex);
    void clearBindings();
    void setEscapeHandler(void (*handler)(void*), void* userData);

    void draw() override;
    int handle(int event) override;

private:
    GameState* mp_gameState = nullptr;
    InputController* mp_inputController = nullptr;
    std::mutex* mp_stateMutex = nullptr;
    void (*mp_escapeHandler)(void*) = nullptr;
    void* mp_escapeData = nullptr;
};
