#include "controller/game_controller.h"

#include "model/player.h"
#include "view/game_view.h"

#include <FL/Fl.H>
#include <FL/Fl_Widget.H>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <mutex>

namespace {

template <typename TAction>
void runControllerAction(void* pData, TAction&& action) {
    auto* pController = static_cast<GameController*>(pData);
    if (!pController) {
        return;
    }
    action(*pController);
}

}

GameController::GameController()
    : m_gameConfig(GameConfig::loadFromFile("assets/config/game.cfg")),
      mp_mainWindow(std::make_unique<MainWindow>(m_gameConfig)),
      mp_gameState(std::make_unique<GameState>(m_gameConfig)),
      mp_inputController(std::make_unique<InputController>()),
      mp_gameLoopController(std::make_unique<GameLoopController>(m_gameConfig.loop.logicHz)) {
    mp_mainWindow->getMenuView()->getNewGameButton()->callback(&GameController::onNewGameClicked, this);
    mp_mainWindow->getMenuView()->getAboutButton()->callback(&GameController::onAboutClicked, this);
    mp_mainWindow->getMenuView()->getExitButton()->callback(&GameController::onExitClicked, this);
    mp_mainWindow->getAboutView()->getBackButton()->callback(&GameController::onBackFromAboutClicked, this);
    mp_mainWindow->getGameOverView()->getBackButton()->callback(&GameController::onBackToMenuClicked, this);
    mp_mainWindow->getGameView()->setStateMutex(&m_stateMutex);
    mp_mainWindow->getHudView()->setStateMutex(&m_stateMutex);
}

GameController::~GameController() {
    unscheduleGameTick();
    mp_gameLoopController->stop();
}

void GameController::run() {
    mp_mainWindow->show();
    Fl::run();
}

void GameController::onNewGameClicked(Fl_Widget* pWidget, void* pData) {
    (void)pWidget;
    runControllerAction(pData, [](GameController& controller) { controller.startGame(); });
}

void GameController::onAboutClicked(Fl_Widget* pWidget, void* pData) {
    (void)pWidget;
    runControllerAction(pData, [](GameController& controller) { controller.showAbout(); });
}

void GameController::onExitClicked(Fl_Widget* pWidget, void* pData) {
    (void)pWidget;
    (void)pData;
    Fl::first_window()->hide();
}

void GameController::onBackFromAboutClicked(Fl_Widget* pWidget, void* pData) {
    (void)pWidget;
    runControllerAction(pData, [](GameController& controller) { controller.showMenu(); });
}

void GameController::onBackToMenuClicked(Fl_Widget* pWidget, void* pData) {
    (void)pWidget;
    runControllerAction(pData, [](GameController& controller) { controller.stopGameAndReturnMenu(); });
}

void GameController::showMenu() {
    unscheduleGameTick();
    clearGameplayBindings();
    mp_mainWindow->getHudView()->clearGameState();
    mp_gameState->setScreenState(GameScreenState::Menu);
    mp_mainWindow->showMenuScreen();
}

void GameController::showAbout() {
    mp_gameState->setScreenState(GameScreenState::About);
    mp_mainWindow->showAboutScreen();
}

void GameController::scheduleGameTick() {
    if (m_gameTickActive) {
        return;
    }
    Fl::add_timeout(1.0 / static_cast<double>(m_gameConfig.loop.renderHz), &GameController::onGameTick, this);
    m_gameTickActive = true;
}

void GameController::unscheduleGameTick() {
    if (!m_gameTickActive) {
        return;
    }
    Fl::remove_timeout(&GameController::onGameTick, this);
    m_gameTickActive = false;
}

void GameController::onGameTick(void* pData) {
    auto* pController = static_cast<GameController*>(pData);
    pController->tickRender();
    if (pController->m_gameTickActive) {
        Fl::repeat_timeout(1.0 / static_cast<double>(pController->m_gameConfig.loop.renderHz), &GameController::onGameTick, pData);
    }
}

void GameController::onEscapeFromGame(void* pData) {
    runControllerAction(pData, [](GameController& controller) { controller.stopGameAndReturnMenu(); });
}

void GameController::tickRender() {
    handleGameOverIfNeeded();
    bool isPlaying = false;
    {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        isPlaying = (mp_gameState->getScreenState() == GameScreenState::Playing);
    }
    if (isPlaying) {
        mp_mainWindow->getGameView()->redraw();
        mp_mainWindow->getHudView()->redraw();
    }
}

void GameController::tickLogic(float deltaSeconds) {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    if (mp_gameState->getScreenState() != GameScreenState::Playing) {
        return;
    }

    const auto pPlayer = mp_gameState->getPlayer();
    const auto pMap = mp_gameState->getMap();
    if (!pPlayer || !pMap) {
        return;
    }

    GameView* pGameView = mp_mainWindow->getGameView();
    if (pPlayer->getHealth() > 0) {
        mp_inputController->tick(
            *pPlayer,
            *mp_gameState,
            *pMap,
            deltaSeconds,
            static_cast<float>(pGameView->w()),
            static_cast<float>(pGameView->h()));
    }

    mp_gameState->updateBots(deltaSeconds);
    mp_gameState->updateBullets(deltaSeconds);
    mp_gameState->updateBonuses(deltaSeconds);
    mp_gameState->updateRespawns(deltaSeconds);
    mp_gameState->updateMatchClock(deltaSeconds);
    Fl::awake();
}

void GameController::handleGameOverIfNeeded() {
    char finalText[96];
    bool mustSwitchToGameOver = false;
    {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        if (mp_gameState->getScreenState() != GameScreenState::Playing) {
            return;
        }
        if (!mp_gameState->hasReachedGameOverCondition()) {
            return;
        }

        const int elapsedSeconds = static_cast<int>(std::floor(mp_gameState->getElapsedMatchSeconds()));
        const int minutes = std::max(0, elapsedSeconds / 60);
        const int seconds = std::max(0, elapsedSeconds % 60);
        std::snprintf(
            finalText,
            sizeof(finalText),
            "Score: %d/%d  Time: %02d:%02d",
            mp_gameState->getPlayerScore(),
            mp_gameState->getScoreLimit(),
            minutes,
            seconds);
        mp_gameState->setScreenState(GameScreenState::GameOver);
        mustSwitchToGameOver = true;
    }
    if (!mustSwitchToGameOver) {
        return;
    }

    mp_gameLoopController->stop();
    unscheduleGameTick();
    clearGameplayBindings();
    mp_mainWindow->getGameOverView()->setFinalText(finalText);
    mp_mainWindow->showGameOverScreen();
}

void GameController::startGame() {
    const char* selectedMapPath = mp_mainWindow->getMenuView()->getSelectedMapPath();
    if (!mp_gameState->beginNewGame(selectedMapPath)) {
        return;
    }

    mp_gameState->setScreenState(GameScreenState::Playing);

    GameView* pGameView = mp_mainWindow->getGameView();
    pGameView->setBindings(mp_gameState.get(), mp_inputController.get());
    pGameView->applyTextureConfig(m_gameConfig.textures);
    pGameView->setEscapeHandler(&GameController::onEscapeFromGame, this);
    mp_inputController->setPlayer(mp_gameState->getPlayer().get());
    mp_inputController->reset(static_cast<float>(pGameView->w()), static_cast<float>(pGameView->h()));

    mp_mainWindow->getHudView()->setGameState(mp_gameState.get());

    mp_gameLoopController->start([this](float deltaSeconds) { tickLogic(deltaSeconds); });
    scheduleGameTick();
    mp_mainWindow->showGameScreen();
    pGameView->take_focus();
}

void GameController::stopGameAndReturnMenu() {
    mp_gameLoopController->stop();
    mp_gameState->clearSession();
    showMenu();
}

void GameController::clearGameplayBindings() {
    mp_inputController->setPlayer(nullptr);
    mp_mainWindow->getGameView()->clearBindings();
}
