#include "view/main_window.h"

#include <algorithm>

MainWindow::MainWindow(const GameConfig& config)
    : Fl_Double_Window(config.window.width, config.window.height, config.window.title.c_str()),
      m_hudHeight(std::max(0, config.window.hudHeight)),
      m_menuView(0, 0, config.window.width, config.window.height, config.menu),
      m_aboutView(0, 0, config.window.width, config.window.height),
      m_gameView(0, 0, config.window.width, config.window.height - m_hudHeight),
      m_hudView(0, config.window.height - m_hudHeight, config.window.width, m_hudHeight),
      m_gameOverView(0, 0, config.window.width, config.window.height) {
    end();

    showMenuScreen();
}

void MainWindow::resize(int X, int Y, int W, int H) {
    Fl_Double_Window::resize(X, Y, W, H);
    const int hh = m_hudHeight;
    const int gameH = std::max(1, H - hh);
    m_menuView.resize(0, 0, W, H);
    m_aboutView.resize(0, 0, W, H);
    m_gameView.resize(0, 0, W, gameH);
    m_hudView.resize(0, gameH, W, hh);
    m_gameOverView.resize(0, 0, W, H);
}

void MainWindow::showMenuScreen() {
    m_menuView.show();
    m_aboutView.hide();
    m_gameView.hide();
    m_hudView.hide();
    m_gameOverView.hide();
    redraw();
}

void MainWindow::showAboutScreen() {
    m_menuView.hide();
    m_aboutView.show();
    m_gameView.hide();
    m_hudView.hide();
    m_gameOverView.hide();
    redraw();
}

void MainWindow::showGameScreen() {
    m_menuView.hide();
    m_aboutView.hide();
    m_gameView.show();
    m_hudView.show();
    m_gameOverView.hide();
    redraw();
}

void MainWindow::showGameOverScreen() {
    m_menuView.hide();
    m_aboutView.hide();
    m_gameView.hide();
    m_hudView.hide();
    m_gameOverView.show();
    redraw();
}

MenuView* MainWindow::getMenuView() {
    return &m_menuView;
}

AboutView* MainWindow::getAboutView() {
    return &m_aboutView;
}

GameView* MainWindow::getGameView() {
    return &m_gameView;
}

HudView* MainWindow::getHudView() {
    return &m_hudView;
}

GameOverView* MainWindow::getGameOverView() {
    return &m_gameOverView;
}
