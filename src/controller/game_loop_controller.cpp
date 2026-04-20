#include "controller/game_loop_controller.h"

#include <chrono>

GameLoopController::~GameLoopController() {
    stop();
}

void GameLoopController::start() {
    stop();
    m_isRunning = true;
    m_logicThread = std::thread(&GameLoopController::loop, this);
}

void GameLoopController::stop() {
    m_isRunning = false;
    if (m_logicThread.joinable()) {
        m_logicThread.join();
    }
}

void GameLoopController::loop() {
    using namespace std::chrono_literals;
    while (m_isRunning) {
        std::this_thread::sleep_for(16ms);
    }
}
