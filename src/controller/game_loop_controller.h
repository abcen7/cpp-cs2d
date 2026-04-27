#pragma once

#include <atomic>
#include <functional>
#include <thread>

class GameLoopController {
public:
    explicit GameLoopController(float tickRateHz = 60.0f);
    ~GameLoopController();

    void start(const std::function<void(float)>& updateCallback);
    void stop();

private:
    void loop();

    std::thread m_logicThread;
    std::atomic<bool> m_isRunning = false;
    std::function<void(float)> m_updateCallback;
    float m_tickRateHz = 60.0f;
};
