#pragma once

#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Group.H>

#include "common/game_config.h"

class MenuView : public Fl_Group {
public:
    MenuView(int x, int y, int width, int height, const MenuConfig& config);
    void resize(int x, int y, int width, int height) override;

    [[nodiscard]] Fl_Button* getNewGameButton();
    [[nodiscard]] Fl_Button* getAboutButton();
    [[nodiscard]] Fl_Button* getExitButton();
    [[nodiscard]] const char* getSelectedMapPath() const;

private:
    void layoutWidgets(int width, int height);

    Fl_Box m_titleBox;
    Fl_Box m_subtitleBox;
    Fl_Box m_panelBox;
    Fl_Button m_newGameButton;
    Fl_Button m_aboutButton;
    Fl_Button m_exitButton;
    Fl_Choice m_mapChoice;
    MenuConfig m_config;
};
