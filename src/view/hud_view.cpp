#include "view/hud_view.h"

#include "model/game_state.h"
#include "model/player.h"

#include <FL/Fl_Image.H>
#include <FL/Fl_SVG_Image.H>
#include <FL/fl_draw.H>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <memory>
#include <mutex>

namespace {

constexpr int HUD_ICON_SIZE = 22;
constexpr unsigned char HUD_ACCENT_R = 253;
constexpr unsigned char HUD_ACCENT_G = 227;
constexpr unsigned char HUD_ACCENT_B = 108;

std::unique_ptr<Fl_Image> loadSvgIcon(const char* path) {
    auto pImage = std::make_unique<Fl_SVG_Image>(path);
    if (!pImage || pImage->fail()) {
        return nullptr;
    }
    pImage->scale(HUD_ICON_SIZE, HUD_ICON_SIZE, 1, 1);
    return pImage;
}

}

HudView::HudView(int x, int y, int width, int height) : Fl_Group(x, y, width, height) {
    mp_healthIcon = loadSvgIcon("assets/icons/health.svg");
    mp_armorIcon = loadSvgIcon("assets/icons/armor.svg");
    mp_ammoIcon = loadSvgIcon("assets/icons/bullet.svg");
    mp_clockIcon = loadSvgIcon("assets/icons/clock.svg");
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

void HudView::drawIconOrFallback(Fl_Image* pIcon, int iconX, int iconY, int iconSize, const char* fallbackText) const {
    if (pIcon != nullptr && pIcon->w() > 0 && pIcon->h() > 0) {
        pIcon->draw(iconX, iconY);
        return;
    }
    fl_color(HUD_ACCENT_R, HUD_ACCENT_G, HUD_ACCENT_B);
    fl_draw(fallbackText, iconX, iconY + iconSize - 6);
}

void HudView::draw() {
    std::unique_lock<std::mutex> stateLock;
    if (mp_stateMutex != nullptr) {
        stateLock = std::unique_lock<std::mutex>(*mp_stateMutex);
    }
    fl_push_clip(x(), y(), w(), h());

    fl_color(10, 12, 16);
    fl_rectf(x(), y(), w(), h());

    char scoreText[48];
    char timeText[16];
    char hpText[16];
    char armorText[16];
    char ammoText[32];
    char weaponText[24];
    char stateSuffix[56];
    stateSuffix[0] = '\0';
    std::snprintf(scoreText, sizeof(scoreText), "Score: --/--");
    std::snprintf(timeText, sizeof(timeText), "--:--");
    std::snprintf(hpText, sizeof(hpText), "--");
    std::snprintf(armorText, sizeof(armorText), "--");
    std::snprintf(ammoText, sizeof(ammoText), "--/--");
    std::snprintf(weaponText, sizeof(weaponText), "[--]");

    if (mp_gameState != nullptr) {
        const auto pPlayer = mp_gameState->getPlayer();
        if (pPlayer) {
            const int elapsedSeconds = static_cast<int>(std::floor(mp_gameState->getElapsedMatchSeconds()));
            const int remainingSeconds = std::max(
                0,
                static_cast<int>(std::ceil(mp_gameState->getMatchDurationLimitSeconds())) - elapsedSeconds);
            const int remMinutes = remainingSeconds / 60;
            const int remSecs = remainingSeconds % 60;
            const bool playerAlive = pPlayer->getHealth() > 0;
            const int respawnTenths = static_cast<int>(std::ceil(mp_gameState->getPlayerRespawnSecondsLeft() * 10.0f));
            const int respawnWhole = respawnTenths / 10;
            const int respawnFrac = respawnTenths % 10;
            std::snprintf(scoreText, sizeof(scoreText), "Score: %d/%d", mp_gameState->getPlayerScore(), mp_gameState->getScoreLimit());
            std::snprintf(timeText, sizeof(timeText), "%02d:%02d", remMinutes, remSecs);
            std::snprintf(hpText, sizeof(hpText), "%d", pPlayer->getHealth());
            std::snprintf(armorText, sizeof(armorText), "%d", pPlayer->getArmor());
            std::snprintf(
                ammoText,
                sizeof(ammoText),
                "%d/%d%s",
                pPlayer->getActiveWeaponAmmoInMagazine(),
                pPlayer->getActiveWeaponMagazineCapacity(),
                pPlayer->isActiveWeaponReloading() ? " [reload]" : "");
            std::snprintf(weaponText, sizeof(weaponText), "[%d]", pPlayer->getActiveWeaponSlot() + 1);

            if (!playerAlive) {
                if (mp_gameState->isPlayerRespawnPending()) {
                    std::snprintf(stateSuffix, sizeof(stateSuffix), " [DEAD respawn: %d.%1ds]", respawnWhole, respawnFrac);
                } else {
                    std::snprintf(stateSuffix, sizeof(stateSuffix), " [DEAD]");
                }
            }
        }
    }

    constexpr int ICON_SIZE = HUD_ICON_SIZE;
    const int baselineY = y() + 36;
    const int iconY = y() + (h() - ICON_SIZE) / 2;

    fl_font(FL_HELVETICA_BOLD, 24);
    fl_color(HUD_ACCENT_R, HUD_ACCENT_G, HUD_ACCENT_B);

    int leftX = x() + 14;
    drawIconOrFallback(mp_healthIcon.get(), leftX, iconY, ICON_SIZE, "+");
    leftX += ICON_SIZE + 8;
    fl_draw(hpText, leftX, baselineY);
    leftX += 56;

    drawIconOrFallback(mp_armorIcon.get(), leftX, iconY, ICON_SIZE, "S");
    leftX += ICON_SIZE + 8;
    fl_draw(armorText, leftX, baselineY);

    fl_font(FL_HELVETICA_BOLD, 28);
    const int timerTextWidth = static_cast<int>(fl_width(timeText));
    const int timerBlockWidth = ICON_SIZE + 10 + timerTextWidth;
    int timerX = x() + (w() - timerBlockWidth) / 2;
    drawIconOrFallback(mp_clockIcon.get(), timerX, iconY, ICON_SIZE, "T");
    timerX += ICON_SIZE + 10;
    fl_draw(timeText, timerX, baselineY);

    fl_font(FL_HELVETICA_BOLD, 22);
    const int ammoTextWidth = static_cast<int>(fl_width(ammoText));
    const int weaponTextWidth = static_cast<int>(fl_width(weaponText));
    int rightX = x() + w() - 14 - (ICON_SIZE + 8 + weaponTextWidth + 10 + ammoTextWidth);
    drawIconOrFallback(mp_ammoIcon.get(), rightX, iconY, ICON_SIZE, "A");
    rightX += ICON_SIZE + 8;
    fl_draw(weaponText, rightX, baselineY);
    rightX += weaponTextWidth + 10;
    fl_draw(ammoText, rightX, baselineY);

    fl_font(FL_HELVETICA, 14);
    fl_color(205, 210, 220);
    fl_draw(scoreText, x() + 16, y() + 16);
    if (stateSuffix[0] != '\0') {
        fl_draw(stateSuffix, x() + w() - 170, y() + 16);
    }

    fl_pop_clip();
}
