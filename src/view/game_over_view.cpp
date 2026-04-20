#include "view/game_over_view.h"

#include <FL/Enumerations.H>

GameOverView::GameOverView(int x, int y, int width, int height)
    : Fl_Group(x, y, width, height),
      m_titleBox(0, 120, width, 40, "Game Over"),
      m_resultBox(0, 180, width, 30, "Score: 0  Time: 00:00"),
      m_backButton((width - 200) / 2, 280, 200, 40, "Back to Menu") {
    m_titleBox.align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
    m_titleBox.labelsize(32);
    m_resultBox.align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
    m_resultBox.labelsize(20);

    end();
}

Fl_Button* GameOverView::getBackButton() {
    return &m_backButton;
}

void GameOverView::setFinalText(const char* text) {
    m_resultBox.label(text);
}
