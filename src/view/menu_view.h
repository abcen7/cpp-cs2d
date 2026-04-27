#pragma once

#include <FL/Fl_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Group.H>

class MenuView : public Fl_Group {
public:
    MenuView(int x, int y, int width, int height);

    [[nodiscard]] Fl_Button* getNewGameButton();
    [[nodiscard]] Fl_Button* getAboutButton();
    [[nodiscard]] Fl_Button* getExitButton();
    [[nodiscard]] const char* getSelectedMapPath() const;

private:
    Fl_Button m_newGameButton;
    Fl_Button m_aboutButton;
    Fl_Button m_exitButton;
    Fl_Choice m_mapChoice;
};
