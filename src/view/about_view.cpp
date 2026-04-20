#include "view/about_view.h"

#include <FL/Enumerations.H>

AboutView::AboutView(int x, int y, int width, int height)
    : Fl_Group(x, y, width, height),
      m_titleBox(0, 60, width, 40, "Counter-Strike 2D Simplified"),
      m_infoBox(
          80,
          120,
          width - 160,
          220,
          "Controls:\n"
          "W/A/S/D - move\n"
          "Mouse - aim\n"
          "Left click - shoot\n"
          "R - reload\n"
          "Esc - pause"),
      m_backButton((width - 200) / 2, 380, 200, 40, "Back") {
    m_titleBox.align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
    m_titleBox.labelsize(24);

    m_infoBox.align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    m_infoBox.box(FL_BORDER_BOX);
    m_infoBox.labelsize(16);

    end();
}

Fl_Button* AboutView::getBackButton() {
    return &m_backButton;
}
