#pragma once

#include <FL/Fl_Image.H>
#include <FL/Fl_Group.H>
#include <memory>
#include <mutex>

class GameState;

class HudView : public Fl_Group {
public:
    HudView(int x, int y, int width, int height);

    void setGameState(GameState* pGameState);
    void setStateMutex(std::mutex* pStateMutex);
    void clearGameState();

    void draw() override;

private:
    void drawIconOrFallback(Fl_Image* pIcon, int x, int y, int size, const char* fallbackText) const;

    GameState* mp_gameState = nullptr;
    std::mutex* mp_stateMutex = nullptr;
    std::unique_ptr<Fl_Image> mp_healthIcon;
    std::unique_ptr<Fl_Image> mp_armorIcon;
    std::unique_ptr<Fl_Image> mp_ammoIcon;
    std::unique_ptr<Fl_Image> mp_clockIcon;
};
