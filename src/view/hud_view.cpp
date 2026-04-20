#include "view/hud_view.h"

#include "model/game_state.h"
#include "model/player.h"

#include <FL/fl_draw.H>

#include <cstdio>

HudView::HudView(int x, int y, int width, int height) : Fl_Group(x, y, width, height) {
    end();
}

void HudView::setGameState(GameState* pGameState) {
    mp_gameState = pGameState;
}

void HudView::clearGameState() {
    mp_gameState = nullptr;
}

void HudView::draw() {
    fl_push_clip(x(), y(), w(), h());

    fl_color(20, 20, 20);
    fl_rectf(x(), y(), w(), h());

    char buffer[160];
    if (mp_gameState != nullptr) {
        const auto pPlayer = mp_gameState->getPlayer();
        if (pPlayer) {
            std::snprintf(
                buffer,
                sizeof(buffer),
                "Score: %d   HP: %d   Armor: %d   [%d] Ammo: %d/%d%s",
                mp_gameState->getPlayerScore(),
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
