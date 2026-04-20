#include "view/menu_view.h"

MenuView::MenuView(int x, int y, int width, int height)
    : Fl_Group(x, y, width, height),
      m_newGameButton((width - 200) / 2, 180, 200, 40, "New Game"),
      m_aboutButton((width - 200) / 2, 240, 200, 40, "About"),
      m_exitButton((width - 200) / 2, 300, 200, 40, "Exit") {
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
