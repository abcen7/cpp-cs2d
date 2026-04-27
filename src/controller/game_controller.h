#pragma once

#include <memory>
#include <mutex>

#include "controller/game_loop_controller.h"
#include "controller/input_controller.h"
#include "common/game_config.h"
#include "model/game_state.h"
#include "view/main_window.h"

class Fl_Widget;

class GameController {
public:
    GameController();
    ~GameController();
    void run();

private:
    static void onNewGameClicked(Fl_Widget* pWidget, void* pData);
    static void onAboutClicked(Fl_Widget* pWidget, void* pData);
    static void onExitClicked(Fl_Widget* pWidget, void* pData);
    static void onBackFromAboutClicked(Fl_Widget* pWidget, void* pData);
    static void onBackToMenuClicked(Fl_Widget* pWidget, void* pData);

    void showMenu();
    void showAbout();
    void startGame();
    void stopGameAndReturnMenu();

    static void onGameTick(void* pData);
    static void onEscapeFromGame(void* pData);
    void tickRender();
    void tickLogic(float deltaSeconds);
    void handleGameOverIfNeeded();
    void scheduleGameTick();
    void unscheduleGameTick();

    GameConfig m_gameConfig;
    std::unique_ptr<MainWindow> mp_mainWindow;
    std::unique_ptr<GameState> mp_gameState;
    std::unique_ptr<InputController> mp_inputController;
    std::unique_ptr<GameLoopController> mp_gameLoopController;
    std::mutex m_stateMutex;

    bool m_gameTickActive = false;
};
