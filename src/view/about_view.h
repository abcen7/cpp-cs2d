#pragma once

#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>

class AboutView : public Fl_Group {
public:
    AboutView(int x, int y, int width, int height);

    [[nodiscard]] Fl_Button* getBackButton();

private:
    Fl_Box m_titleBox;
    Fl_Box m_infoBox;
    Fl_Button m_backButton;
};
