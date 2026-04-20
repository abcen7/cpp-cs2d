#pragma once

#include <FL/Fl_Double_Window.H>

#include "view/about_view.h"
#include "view/game_over_view.h"
#include "view/game_view.h"
#include "view/hud_view.h"
#include "view/menu_view.h"

class MainWindow : public Fl_Double_Window {
public:
    MainWindow(int width, int height, const char* title);

    void showMenuScreen();
    void showAboutScreen();
    void showGameScreen();
    void showGameOverScreen();

    [[nodiscard]] MenuView* getMenuView();
    [[nodiscard]] AboutView* getAboutView();
    [[nodiscard]] GameView* getGameView();
    [[nodiscard]] HudView* getHudView();
    [[nodiscard]] GameOverView* getGameOverView();

private:
    MenuView m_menuView;
    AboutView m_aboutView;
    GameView m_gameView;
    HudView m_hudView;
    GameOverView m_gameOverView;
};
