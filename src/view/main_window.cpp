#include "view/main_window.h"

MainWindow::MainWindow(int width, int height, const char* title)
    : Fl_Double_Window(width, height, title),
      m_menuView(0, 0, width, height),
      m_aboutView(0, 0, width, height),
      m_gameView(0, 0, width, height - 56),
      m_hudView(0, height - 56, width, 56),
      m_gameOverView(0, 0, width, height) {
    end();

    showMenuScreen();
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
