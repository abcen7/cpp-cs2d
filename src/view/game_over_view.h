#pragma once

#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>

class GameOverView : public Fl_Group {
public:
    GameOverView(int x, int y, int width, int height);
    void resize(int x, int y, int width, int height) override;

    [[nodiscard]] Fl_Button* getBackButton();
    void setFinalText(const char* text);

private:
    void layoutWidgets(int width, int height);

    Fl_Box m_titleBox;
    Fl_Box m_subtitleBox;
    Fl_Box m_cardBox;
    Fl_Box m_resultBox;
    Fl_Button m_backButton;
};
