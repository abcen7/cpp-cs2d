#include "view/menu_view.h"

#include <FL/Enumerations.H>

MenuView::MenuView(int x, int y, int width, int height, const MenuConfig& config)
    : Fl_Group(x, y, width, height),
      m_titleBox(0, 0, width, 48, "Counter-Strike 2D"),
      m_subtitleBox(0, 0, width, 24, "Select map and jump into action"),
      m_panelBox(0, 0, 360, 300),
      m_mapChoice(0, 0, 240, 34, "Map"),
      m_newGameButton(0, 0, 240, 42, "New Game"),
      m_aboutButton(0, 0, 240, 42, "About"),
      m_exitButton(0, 0, 240, 42, "Exit"),
      m_config(config) {
    box(FL_FLAT_BOX);
    color(fl_rgb_color(18, 22, 30));

    m_titleBox.align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
    m_titleBox.labelfont(FL_HELVETICA_BOLD);
    m_titleBox.labelsize(34);
    m_titleBox.labelcolor(fl_rgb_color(240, 244, 255));

    m_subtitleBox.align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
    m_subtitleBox.labelsize(14);
    m_subtitleBox.labelcolor(fl_rgb_color(156, 168, 190));

    m_panelBox.box(FL_BORDER_BOX);
    m_panelBox.color(fl_rgb_color(31, 37, 52));
    m_panelBox.labelcolor(fl_rgb_color(31, 37, 52));

    m_mapChoice.labelfont(FL_HELVETICA_BOLD);
    m_mapChoice.labelsize(14);
    m_mapChoice.textsize(14);
    m_mapChoice.down_box(FL_BORDER_BOX);
    m_mapChoice.align(FL_ALIGN_TOP_LEFT);

    m_newGameButton.box(FL_UP_BOX);
    m_newGameButton.color(fl_rgb_color(46, 125, 220));
    m_newGameButton.labelcolor(FL_WHITE);
    m_newGameButton.labelsize(16);
    m_newGameButton.labelfont(FL_HELVETICA_BOLD);

    m_aboutButton.box(FL_UP_BOX);
    m_aboutButton.color(fl_rgb_color(70, 79, 104));
    m_aboutButton.labelcolor(FL_WHITE);
    m_aboutButton.labelsize(15);

    m_exitButton.box(FL_UP_BOX);
    m_exitButton.color(fl_rgb_color(112, 62, 72));
    m_exitButton.labelcolor(FL_WHITE);
    m_exitButton.labelsize(15);

    m_mapChoice.add("de_simple (test)");
    m_mapChoice.add("de_crossfire");
    m_mapChoice.add("de_maze_run");
    m_mapChoice.add("de_arena_plus");
    m_mapChoice.value(0);

    layoutWidgets(width, height);
    end();
}

void MenuView::resize(int x, int y, int width, int height) {
    Fl_Group::resize(x, y, width, height);
    layoutWidgets(width, height);
}

Fl_Button* MenuView::getNewGameButton() {
    return &m_newGameButton;
}

Fl_Button* MenuView::getAboutButton() {
    return &m_aboutButton;
}

Fl_Button* MenuView::getExitButton() {
    return &m_exitButton;
}

const char* MenuView::getSelectedMapPath() const {
    switch (m_mapChoice.value()) {
    case 1:
        return m_config.crossfireMapPath.c_str();
    case 2:
        return m_config.mazeRunMapPath.c_str();
    case 3:
        return m_config.arenaPlusMapPath.c_str();
    default:
        return m_config.simpleMapPath.c_str();
    }
}

void MenuView::layoutWidgets(int width, int height) {
    constexpr int panelWidth = 360;
    constexpr int panelHeight = 300;
    const int panelX = (width - panelWidth) / 2;
    const int panelY = (height - panelHeight) / 2 + 36;

    m_titleBox.resize(0, 44, width, 48);
    m_subtitleBox.resize(0, 92, width, 24);
    m_panelBox.resize(panelX, panelY, panelWidth, panelHeight);

    m_mapChoice.resize(panelX + 60, panelY + 38, 240, 34);
    m_newGameButton.resize(panelX + 60, panelY + 102, 240, 42);
    m_aboutButton.resize(panelX + 60, panelY + 162, 240, 42);
    m_exitButton.resize(panelX + 60, panelY + 222, 240, 42);
}
