#include "view/hud_view.h"

#include "model/game_state.h"
#include "model/player.h"

#include <FL/fl_draw.H>

#include <algorithm>
#include <cmath>
#include <mutex>
#include <cstdio>

HudView::HudView(int x, int y, int width, int height) : Fl_Group(x, y, width, height) {
    end();
}

void HudView::setGameState(GameState* pGameState) {
    mp_gameState = pGameState;
}

void HudView::setStateMutex(std::mutex* pStateMutex) {
    mp_stateMutex = pStateMutex;
}

void HudView::clearGameState() {
    mp_gameState = nullptr;
}

void HudView::draw() {
    std::unique_lock<std::mutex> stateLock;
    if (mp_stateMutex != nullptr) {
        stateLock = std::unique_lock<std::mutex>(*mp_stateMutex);
    }
    fl_push_clip(x(), y(), w(), h());

    fl_color(20, 20, 20);
    fl_rectf(x(), y(), w(), h());

    char buffer[160];
    if (mp_gameState != nullptr) {
        const auto pPlayer = mp_gameState->getPlayer();
        if (pPlayer) {
            const int elapsedSeconds = static_cast<int>(std::floor(mp_gameState->getElapsedMatchSeconds()));
            const int remainingSeconds = std::max(
                0,
                static_cast<int>(std::ceil(mp_gameState->getMatchDurationLimitSeconds())) - elapsedSeconds);
            const int remMinutes = remainingSeconds / 60;
            const int remSecs = remainingSeconds % 60;
            std::snprintf(
                buffer,
                sizeof(buffer),
                "Score: %d/%d   Time: %02d:%02d   HP: %d   Armor: %d   [%d] Ammo: %d/%d%s",
                mp_gameState->getPlayerScore(),
                mp_gameState->getScoreLimit(),
                remMinutes,
                remSecs,
                pPlayer->getHealth(),
                pPlayer->getArmor(),
                pPlayer->getActiveWeaponSlot() + 1,
                pPlayer->getActiveWeaponAmmoInMagazine(),
                pPlayer->getActiveWeaponMagazineCapacity(),
                pPlayer->isActiveWeaponReloading() ? "  [reload]" : "");
        } else {
            std::snprintf(buffer, sizeof(buffer), "HP: --   Armor: --   Ammo: --/--");
        }
    } else {
        std::snprintf(buffer, sizeof(buffer), "HP: --   Armor: --   Ammo: --/--");
    }

    fl_color(240, 240, 240);
    fl_draw(buffer, x() + 16, y() + 24);

    fl_pop_clip();
}
