#pragma once

#include <atomic>
#include <thread>

class GameLoopController {
public:
    GameLoopController() = default;
    ~GameLoopController();

    void start();
    void stop();

private:
    void loop();

    std::thread m_logicThread;
    std::atomic<bool> m_isRunning = false;
};
