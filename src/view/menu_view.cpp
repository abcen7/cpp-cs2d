#include "view/menu_view.h"

MenuView::MenuView(int x, int y, int width, int height, const MenuConfig& config)
    : Fl_Group(x, y, width, height),
      m_mapChoice((width - 280) / 2, 130, 280, 32, "Map:"),
      m_newGameButton((width - 200) / 2, 180, 200, 40, "New Game"),
      m_aboutButton((width - 200) / 2, 240, 200, 40, "About"),
      m_exitButton((width - 200) / 2, 300, 200, 40, "Exit"),
      m_config(config) {
    m_mapChoice.add("de_simple (test)");
    m_mapChoice.add("de_crossfire");
    m_mapChoice.add("de_maze_run");
    m_mapChoice.add("de_arena_plus");
    m_mapChoice.value(0);
    end();
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
