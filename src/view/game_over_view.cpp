#include "view/game_over_view.h"

#include <FL/Enumerations.H>
#include <FL/fl_draw.H>

GameOverView::GameOverView(int x, int y, int width, int height)
    : Fl_Group(x, y, width, height),
      m_titleBox(0, 0, width, 52, "Match Finished"),
      m_subtitleBox(0, 0, width, 24, "Game Over"),
      m_cardBox(0, 0, 440, 220),
      m_resultBox(0, 0, 360, 44, "Score: 0  Time: 00:00"),
      m_backButton(0, 0, 220, 42, "Back to Menu") {
    box(FL_FLAT_BOX);
    color(fl_rgb_color(18, 22, 30));

    m_titleBox.align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
    m_titleBox.labelfont(FL_HELVETICA_BOLD);
    m_titleBox.labelsize(34);
    m_titleBox.labelcolor(fl_rgb_color(240, 244, 255));

    m_subtitleBox.align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
    m_subtitleBox.labelsize(14);
    m_subtitleBox.labelcolor(fl_rgb_color(156, 168, 190));

    m_cardBox.box(FL_BORDER_BOX);
    m_cardBox.color(fl_rgb_color(31, 37, 52));
    m_cardBox.labelcolor(fl_rgb_color(31, 37, 52));

    m_resultBox.align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
    m_resultBox.box(FL_BORDER_BOX);
    m_resultBox.color(fl_rgb_color(24, 29, 42));
    m_resultBox.labelcolor(fl_rgb_color(244, 248, 255));
    m_resultBox.labelfont(FL_HELVETICA_BOLD);
    m_resultBox.labelsize(20);

    m_backButton.box(FL_UP_BOX);
    m_backButton.color(fl_rgb_color(70, 79, 104));
    m_backButton.labelcolor(FL_WHITE);
    m_backButton.labelsize(15);

    layoutWidgets(width, height);
    end();
}

Fl_Button* GameOverView::getBackButton() {
    return &m_backButton;
}

void GameOverView::setFinalText(const char* text) {
    m_resultBox.label(text);
}

void GameOverView::resize(int x, int y, int width, int height) {
    Fl_Group::resize(x, y, width, height);
    layoutWidgets(width, height);
}

void GameOverView::layoutWidgets(int width, int height) {
    constexpr int cardWidth = 440;
    constexpr int cardHeight = 220;

    const int cardX = (width - cardWidth) / 2;
    const int cardY = (height - cardHeight) / 2 + 28;

    m_titleBox.resize(0, 52, width, 52);
    m_subtitleBox.resize(0, 104, width, 24);
    m_cardBox.resize(cardX, cardY, cardWidth, cardHeight);
    m_resultBox.resize(cardX + 40, cardY + 44, cardWidth - 80, 52);
    m_backButton.resize(cardX + (cardWidth - 220) / 2, cardY + 136, 220, 42);
}
