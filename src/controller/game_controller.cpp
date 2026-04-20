#include "controller/game_controller.h"

#include "view/game_view.h"

#include <FL/Fl.H>
#include <FL/Fl_Widget.H>

GameController::GameController()
    : mp_mainWindow(std::make_unique<MainWindow>(1024, 768, "CS2DS")),
      mp_gameState(std::make_unique<GameState>()),
      mp_inputController(std::make_unique<InputController>()),
      mp_gameLoopController(std::make_unique<GameLoopController>()) {
    mp_mainWindow->getMenuView()->getNewGameButton()->callback(&GameController::onNewGameClicked, this);
    mp_mainWindow->getMenuView()->getAboutButton()->callback(&GameController::onAboutClicked, this);
    mp_mainWindow->getMenuView()->getExitButton()->callback(&GameController::onExitClicked, this);
    mp_mainWindow->getAboutView()->getBackButton()->callback(&GameController::onBackFromAboutClicked, this);
    mp_mainWindow->getGameOverView()->getBackButton()->callback(&GameController::onBackToMenuClicked, this);
}

GameController::~GameController() {
    unscheduleGameTick();
}

void GameController::run() {
    mp_mainWindow->show();
    Fl::run();
}

void GameController::onNewGameClicked(Fl_Widget* pWidget, void* pData) {
    (void)pWidget;
    auto* pController = static_cast<GameController*>(pData);
    pController->startGame();
}

void GameController::onAboutClicked(Fl_Widget* pWidget, void* pData) {
    (void)pWidget;
    auto* pController = static_cast<GameController*>(pData);
    pController->showAbout();
}

void GameController::onExitClicked(Fl_Widget* pWidget, void* pData) {
    (void)pWidget;
    (void)pData;
    Fl::first_window()->hide();
}

void GameController::onBackFromAboutClicked(Fl_Widget* pWidget, void* pData) {
    (void)pWidget;
    auto* pController = static_cast<GameController*>(pData);
    pController->showMenu();
}

void GameController::onBackToMenuClicked(Fl_Widget* pWidget, void* pData) {
    (void)pWidget;
    auto* pController = static_cast<GameController*>(pData);
    pController->stopGameAndReturnMenu();
}

void GameController::showMenu() {
    unscheduleGameTick();
    mp_inputController->setPlayer(nullptr);
    mp_mainWindow->getGameView()->clearBindings();
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
    Fl::add_timeout(1.0 / 60.0, &GameController::onGameTick, this);
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
    pController->tickGame();
    Fl::repeat_timeout(1.0 / 60.0, &GameController::onGameTick, pData);
}

void GameController::onEscapeFromGame(void* pData) {
    auto* pController = static_cast<GameController*>(pData);
    pController->stopGameAndReturnMenu();
}

void GameController::tickGame() {
    if (mp_gameState->getScreenState() != GameScreenState::Playing) {
        return;
    }

    const auto pPlayer = mp_gameState->getPlayer();
    const auto pMap = mp_gameState->getMap();
    if (!pPlayer || !pMap) {
        return;
    }

    GameView* pGameView = mp_mainWindow->getGameView();
    const float deltaSeconds = 1.0f / 60.0f;
    mp_inputController->tick(
        *pPlayer,
        *mp_gameState,
        *pMap,
        deltaSeconds,
        static_cast<float>(pGameView->w()),
        static_cast<float>(pGameView->h()));

    mp_gameState->updateBots(deltaSeconds);
    mp_gameState->updateBullets(deltaSeconds);

    pGameView->redraw();
    mp_mainWindow->getHudView()->redraw();
}

void GameController::startGame() {
    static constexpr const char* DEFAULT_MAP_PATH = "assets/maps/de_simple.map";
    if (!mp_gameState->beginNewGame(DEFAULT_MAP_PATH)) {
        return;
    }

    mp_gameState->setScreenState(GameScreenState::Playing);

    GameView* pGameView = mp_mainWindow->getGameView();
    pGameView->setBindings(mp_gameState.get(), mp_inputController.get());
    pGameView->setEscapeHandler(&GameController::onEscapeFromGame, this);
    mp_inputController->setPlayer(mp_gameState->getPlayer().get());
    mp_inputController->reset(static_cast<float>(pGameView->w()), static_cast<float>(pGameView->h()));

    mp_mainWindow->getHudView()->setGameState(mp_gameState.get());

    scheduleGameTick();
    mp_mainWindow->showGameScreen();
    pGameView->take_focus();
}

void GameController::stopGameAndReturnMenu() {
    mp_gameLoopController->stop();
    mp_gameState->clearSession();
    showMenu();
}
