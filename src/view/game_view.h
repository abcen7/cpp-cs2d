#pragma once

#include <FL/Fl_Group.H>
#include <FL/Fl_PNG_Image.H>
#include <memory>
#include <mutex>

#include "common/game_config.h"

class GameState;
class InputController;

class GameView : public Fl_Group {
public:
    GameView(int x, int y, int width, int height);

    void setBindings(GameState* pGameState, InputController* pInputController);
    void setStateMutex(std::mutex* pStateMutex);
    void clearBindings();
    void setEscapeHandler(void (*handler)(void*), void* userData);
    void applyTextureConfig(const TextureConfig& config);

    void draw() override;
    int handle(int event) override;

private:
    GameState* mp_gameState = nullptr;
    InputController* mp_inputController = nullptr;
    std::mutex* mp_stateMutex = nullptr;
    void (*mp_escapeHandler)(void*) = nullptr;
    void* mp_escapeData = nullptr;
    bool m_texturesEnabled = false;
    int m_playerTextureSize = 32;
    int m_botTextureSize = 32;
    std::unique_ptr<Fl_PNG_Image> mp_playerTexture;
    std::unique_ptr<Fl_PNG_Image> mp_botTexture;
};
