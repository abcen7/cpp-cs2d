#include "controller/game_loop_controller.h"

#include <chrono>
#include <algorithm>
#include <utility>

GameLoopController::GameLoopController(float tickRateHz) : m_tickRateHz(std::max(1.0f, tickRateHz)) {}

GameLoopController::~GameLoopController() {
    stop();
}

void GameLoopController::start(const std::function<void(float)>& updateCallback) {
    stop();
    m_updateCallback = updateCallback;
    m_isRunning = true;
    m_logicThread = std::thread(&GameLoopController::loop, this);
}

void GameLoopController::stop() {
    m_isRunning = false;
    if (m_logicThread.joinable()) {
        m_logicThread.join();
    }
    m_updateCallback = nullptr;
}

void GameLoopController::loop() {
    using clock = std::chrono::steady_clock;
    using secondsf = std::chrono::duration<float>;
    const float targetStepSeconds = 1.0f / m_tickRateHz;
    const auto sleepDuration = std::chrono::duration_cast<clock::duration>(secondsf(targetStepSeconds));
    auto previous = clock::now();

    while (m_isRunning) {
        const auto now = clock::now();
        const float deltaSeconds = std::chrono::duration_cast<secondsf>(now - previous).count();
        previous = now;

        if (m_updateCallback) {
            m_updateCallback(deltaSeconds > 0.0f ? deltaSeconds : targetStepSeconds);
        }
        std::this_thread::sleep_for(sleepDuration);
    }
}
