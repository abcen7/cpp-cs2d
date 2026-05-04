#include "view/about_view.h"

#include <FL/Enumerations.H>
#include <FL/fl_draw.H>

AboutView::AboutView(int x, int y, int width, int height)
    : Fl_Group(x, y, width, height),
      m_titleBox(0, 0, width, 46, "About"),
      m_subtitleBox(0, 0, width, 22, "Counter-Strike 2D Simplified"),
      m_cardBox(0, 0, 520, 320),
      m_infoBox(
          0,
          0,
          420,
          180,
          "Controls:\n"
          "  W/A/S/D  - move\n"
          "  Mouse    - aim\n"
          "  Left Click - shoot\n"
          "  R        - reload\n"
          "  Esc      - pause"),
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

    m_infoBox.align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    m_infoBox.box(FL_NO_BOX);
    m_infoBox.labelfont(FL_HELVETICA);
    m_infoBox.labelsize(18);
    m_infoBox.labelcolor(fl_rgb_color(226, 232, 242));
    m_infoBox.color(fl_rgb_color(31, 37, 52));

    m_backButton.box(FL_UP_BOX);
    m_backButton.color(fl_rgb_color(70, 79, 104));
    m_backButton.labelcolor(FL_WHITE);
    m_backButton.labelsize(15);

    layoutWidgets(width, height);
    end();
}

Fl_Button* AboutView::getBackButton() {
    return &m_backButton;
}

void AboutView::resize(int x, int y, int width, int height) {
    Fl_Group::resize(x, y, width, height);
    layoutWidgets(width, height);
}

void AboutView::layoutWidgets(int width, int height) {
    constexpr int cardWidth = 520;
    constexpr int cardHeight = 320;

    const int cardX = (width - cardWidth) / 2;
    const int cardY = (height - cardHeight) / 2 + 26;

    m_titleBox.resize(0, 42, width, 46);
    m_subtitleBox.resize(0, 90, width, 22);
    m_cardBox.resize(cardX, cardY, cardWidth, cardHeight);
    m_infoBox.resize(cardX + 46, cardY + 44, cardWidth - 92, 190);
    m_backButton.resize(cardX + (cardWidth - 220) / 2, cardY + 250, 220, 42);
}
