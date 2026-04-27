#include "view/game_view.h"

#include "controller/input_controller.h"

#include "model/bonus.h"
#include "model/bullet.h"
#include "model/bot.h"
#include "model/game_map.h"
#include "model/game_state.h"
#include "model/player.h"
#include "model/tile.h"

#include <FL/Enumerations.H>
#include <FL/Fl.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_RGB_Image.H>
#include <FL/fl_draw.H>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <mutex>
#include <vector>

namespace {

constexpr float kTextureAngleOffsetRadians = -1.5707963267948966f;

void setTileColor(TileType type) {
    switch (type) {
        case TileType::Wall:
            fl_color(35, 38, 48);
            break;
        case TileType::Floor:
            fl_color(52, 56, 64);
            break;
        case TileType::SpawnPlayer:
            fl_color(48, 58, 52);
            break;
        case TileType::SpawnBot:
            fl_color(58, 48, 48);
            break;
        case TileType::BonusSpot:
            fl_color(52, 56, 64);
            break;
    }
}

void drawRotatedTexture(Fl_PNG_Image* pTexture, float centerX, float centerY, int targetSize, float angleRadians) {
    if (pTexture == nullptr || targetSize <= 0) {
        return;
    }

    const char* const* pData = pTexture->data();
    if (pTexture->count() <= 0 || pData == nullptr || pData[0] == nullptr) {
        const int half = targetSize / 2;
        pTexture->draw(static_cast<int>(centerX) - half, static_cast<int>(centerY) - half);
        return;
    }

    const int sourceWidth = pTexture->data_w();
    const int sourceHeight = pTexture->data_h();
    const int sourceDepth = pTexture->d();
    if (sourceWidth <= 0 || sourceHeight <= 0 || sourceDepth <= 0) {
        return;
    }

    const auto* source = reinterpret_cast<const unsigned char*>(pData[0]);
    const int sourceStride = pTexture->ld() > 0 ? pTexture->ld() : sourceWidth * sourceDepth;
    std::vector<unsigned char> pixels(static_cast<size_t>(targetSize * targetSize * 4), 0);

    const float correctedAngle = angleRadians + kTextureAngleOffsetRadians;
    const float cosAngle = std::cos(correctedAngle);
    const float sinAngle = std::sin(correctedAngle);
    const float destCenter = (static_cast<float>(targetSize) - 1.0f) * 0.5f;
    const float sourceCenterX = (static_cast<float>(sourceWidth) - 1.0f) * 0.5f;
    const float sourceCenterY = (static_cast<float>(sourceHeight) - 1.0f) * 0.5f;
    const float sourceScaleX = static_cast<float>(sourceWidth) / static_cast<float>(targetSize);
    const float sourceScaleY = static_cast<float>(sourceHeight) / static_cast<float>(targetSize);

    for (int y = 0; y < targetSize; ++y) {
        for (int x = 0; x < targetSize; ++x) {
            const float dx = static_cast<float>(x) - destCenter;
            const float dy = static_cast<float>(y) - destCenter;
            const int sourceX = static_cast<int>(std::round((cosAngle * dx + sinAngle * dy) * sourceScaleX + sourceCenterX));
            const int sourceY = static_cast<int>(std::round((-sinAngle * dx + cosAngle * dy) * sourceScaleY + sourceCenterY));
            if (sourceX < 0 || sourceX >= sourceWidth || sourceY < 0 || sourceY >= sourceHeight) {
                continue;
            }

            const auto* sourcePixel = source + sourceY * sourceStride + sourceX * sourceDepth;
            auto* targetPixel = pixels.data() + static_cast<size_t>((y * targetSize + x) * 4);
            if (sourceDepth == 1) {
                targetPixel[0] = sourcePixel[0];
                targetPixel[1] = sourcePixel[0];
                targetPixel[2] = sourcePixel[0];
                targetPixel[3] = 255;
                continue;
            }

            targetPixel[0] = sourcePixel[0];
            targetPixel[1] = sourceDepth > 1 ? sourcePixel[1] : sourcePixel[0];
            targetPixel[2] = sourceDepth > 2 ? sourcePixel[2] : sourcePixel[0];
            targetPixel[3] = sourceDepth > 3 ? sourcePixel[3] : 255;
        }
    }

    Fl_RGB_Image rotatedImage(reinterpret_cast<const uchar*>(pixels.data()), targetSize, targetSize, 4);
    const int half = targetSize / 2;
    rotatedImage.draw(static_cast<int>(centerX) - half, static_cast<int>(centerY) - half);
}

} // namespace

GameView::GameView(int x, int y, int width, int height) : Fl_Group(x, y, width, height) {
    box(FL_NO_BOX);
    end();
}

void GameView::applyTextureConfig(const TextureConfig& config) {
    m_texturesEnabled = config.enabled;
    m_playerTextureSize = config.playerSize;
    m_botTextureSize = config.botSize;
    mp_playerTexture.reset();
    mp_botTexture.reset();
    if (!m_texturesEnabled) {
        return;
    }

    auto loadPng = [](const std::string& label, const std::string& path, int targetSize) -> std::unique_ptr<Fl_PNG_Image> {
        if (path.empty()) {
            std::fprintf(stderr, "[textures] %s: path is empty\n", label.c_str());
            return nullptr;
        }
        auto image = std::make_unique<Fl_PNG_Image>(path.c_str());
        if (image->fail() || image->w() <= 0 || image->h() <= 0) {
            std::fprintf(stderr, "[textures] %s: failed to load '%s'\n", label.c_str(), path.c_str());
            return nullptr;
        }
        if (targetSize > 0) {
            image->scale(targetSize, targetSize, 1, 0);
        }
        std::fprintf(stderr, "[textures] %s: loaded '%s' (source %dx%d, scaled to %d)\n",
            label.c_str(), path.c_str(), image->data_w(), image->data_h(), targetSize);
        return image;
    };

    mp_playerTexture = loadPng("player", config.playerPath, m_playerTextureSize);
    mp_botTexture = loadPng("bot", config.botPath, m_botTextureSize);
}

void GameView::setBindings(GameState* pGameState, InputController* pInputController) {
    mp_gameState = pGameState;
    mp_inputController = pInputController;
}

void GameView::setStateMutex(std::mutex* pStateMutex) {
    mp_stateMutex = pStateMutex;
}

void GameView::clearBindings() {
    mp_gameState = nullptr;
    mp_inputController = nullptr;
    mp_escapeHandler = nullptr;
    mp_escapeData = nullptr;
}

void GameView::setEscapeHandler(void (*handler)(void*), void* userData) {
    mp_escapeHandler = handler;
    mp_escapeData = userData;
}

void GameView::draw() {
    std::unique_lock<std::mutex> stateLock;
    if (mp_stateMutex != nullptr) {
        stateLock = std::unique_lock<std::mutex>(*mp_stateMutex);
    }
    fl_push_clip(x(), y(), w(), h());

    fl_color(20, 22, 28);
    fl_rectf(x(), y(), w(), h());

    if (mp_gameState == nullptr) {
        fl_color(200, 200, 200);
        fl_draw("No game state", x() + 20, y() + 40);
        fl_pop_clip();
        return;
    }

    const auto pMap = mp_gameState->getMap();
    const auto pPlayer = mp_gameState->getPlayer();
    if (!pMap || !pPlayer) {
        fl_color(200, 200, 200);
        fl_draw("Loading...", x() + 20, y() + 40);
        fl_pop_clip();
        return;
    }

    const float playerX = pPlayer->getPositionX();
    const float playerY = pPlayer->getPositionY();
    const float viewW = static_cast<float>(w());
    const float viewH = static_cast<float>(h());

    float cameraX = playerX - viewW * 0.5f;
    float cameraY = playerY - viewH * 0.5f;

    const float maxCamX = std::max(0.0f, pMap->getWidthPixels() - viewW);
    const float maxCamY = std::max(0.0f, pMap->getHeightPixels() - viewH);
    cameraX = std::clamp(cameraX, 0.0f, maxCamX);
    cameraY = std::clamp(cameraY, 0.0f, maxCamY);

    const int minGX = std::max(0, static_cast<int>(std::floor(cameraX / static_cast<float>(GameMap::TILE_SIZE))));
    const int maxGX = std::min(
        pMap->getWidthInTiles() - 1,
        static_cast<int>(std::floor((cameraX + viewW - 1.0f) / static_cast<float>(GameMap::TILE_SIZE))));
    const int minGY = std::max(0, static_cast<int>(std::floor(cameraY / static_cast<float>(GameMap::TILE_SIZE))));
    const int maxGY = std::min(
        pMap->getHeightInTiles() - 1,
        static_cast<int>(std::floor((cameraY + viewH - 1.0f) / static_cast<float>(GameMap::TILE_SIZE))));

    for (int gy = minGY; gy <= maxGY; ++gy) {
        for (int gx = minGX; gx <= maxGX; ++gx) {
            const Tile& tile = pMap->getTile(gx, gy);
            const float worldLeft = static_cast<float>(gx * GameMap::TILE_SIZE);
            const float worldTop = static_cast<float>(gy * GameMap::TILE_SIZE);
            const float screenLeft = static_cast<float>(x()) + (worldLeft - cameraX);
            const float screenTop = static_cast<float>(y()) + (worldTop - cameraY);

            setTileColor(tile.getType());
            fl_rectf(
                screenLeft,
                screenTop,
                static_cast<float>(GameMap::TILE_SIZE),
                static_cast<float>(GameMap::TILE_SIZE));
        }
    }

    for (const auto& pBonus : mp_gameState->getBonuses()) {
        if (!pBonus || !pBonus->isActive()) {
            continue;
        }
        const float bonusWorldX = pBonus->getPositionX();
        const float bonusWorldY = pBonus->getPositionY();
        const float bonusScreenX = static_cast<float>(x()) + (bonusWorldX - cameraX);
        const float bonusScreenY = static_cast<float>(y()) + (bonusWorldY - cameraY);
        const float halfW = pBonus->getWidth() * 0.5f;

        switch (pBonus->getKind()) {
            case BonusKind::Health:
                fl_color(80, 220, 120);
                break;
            case BonusKind::Ammo:
                fl_color(255, 200, 90);
                break;
            case BonusKind::Armor:
                fl_color(100, 160, 255);
                break;
        }
        fl_rectf(
            static_cast<int>(bonusScreenX - halfW),
            static_cast<int>(bonusScreenY - pBonus->getHeight() * 0.5f),
            static_cast<int>(pBonus->getWidth()),
            static_cast<int>(pBonus->getHeight()));
        const char* label = "+";
        if (pBonus->getKind() == BonusKind::Ammo) {
            label = "A";
        } else if (pBonus->getKind() == BonusKind::Armor) {
            label = "S";
        }
        fl_color(10, 10, 10);
        fl_draw(label, static_cast<int>(bonusScreenX - 4), static_cast<int>(bonusScreenY + 5));
    }

    const float playerScreenX = static_cast<float>(x()) + (playerX - cameraX);
    const float playerScreenY = static_cast<float>(y()) + (playerY - cameraY);

    const float radius = pPlayer->getWidth() * 0.5f;
    if (m_texturesEnabled && mp_playerTexture) {
        drawRotatedTexture(
            mp_playerTexture.get(),
            playerScreenX,
            playerScreenY,
            m_playerTextureSize,
            pPlayer->getAimAngleRadians());
    } else {
        fl_color(70, 210, 95);
        fl_pie(
            static_cast<int>(playerScreenX - radius),
            static_cast<int>(playerScreenY - radius),
            static_cast<int>(pPlayer->getWidth()),
            static_cast<int>(pPlayer->getHeight()),
            0.0,
            360.0);
    }

    const float aimAngle = pPlayer->getAimAngleRadians();
    const float lineLen = 22.0f;
    const float x2 = playerScreenX + std::cos(aimAngle) * lineLen;
    const float y2 = playerScreenY + std::sin(aimAngle) * lineLen;
    fl_color(255, 255, 255);
    fl_line(static_cast<int>(playerScreenX), static_cast<int>(playerScreenY), static_cast<int>(x2), static_cast<int>(y2));

    for (const auto& pBot : mp_gameState->getBots()) {
        if (!pBot || pBot->getHealth() <= 0) {
            continue;
        }
        const float botScreenX = static_cast<float>(x()) + (pBot->getPositionX() - cameraX);
        const float botScreenY = static_cast<float>(y()) + (pBot->getPositionY() - cameraY);
        const float botRadius = pBot->getWidth() * 0.5f;
        const float botAim = pBot->getAimAngleRadians();

        if (m_texturesEnabled && mp_botTexture) {
            drawRotatedTexture(
                mp_botTexture.get(),
                botScreenX,
                botScreenY,
                m_botTextureSize,
                botAim);
        } else {
            fl_color(220, 80, 80);
            fl_pie(
                static_cast<int>(botScreenX - botRadius),
                static_cast<int>(botScreenY - botRadius),
                static_cast<int>(pBot->getWidth()),
                static_cast<int>(pBot->getHeight()),
                0.0,
                360.0);
        }

        const float botLineX = botScreenX + std::cos(botAim) * 18.0f;
        const float botLineY = botScreenY + std::sin(botAim) * 18.0f;
        fl_color(255, 220, 220);
        fl_line(static_cast<int>(botScreenX), static_cast<int>(botScreenY), static_cast<int>(botLineX), static_cast<int>(botLineY));
    }

    for (const auto& pBullet : mp_gameState->getBullets()) {
        if (!pBullet || !pBullet->isAlive()) {
            continue;
        }
        const float bulletWorldX = pBullet->getPositionX();
        const float bulletWorldY = pBullet->getPositionY();
        const float bulletScreenX = static_cast<float>(x()) + (bulletWorldX - cameraX);
        const float bulletScreenY = static_cast<float>(y()) + (bulletWorldY - cameraY);
        const float bulletRadius = pBullet->getWidth() * 0.5f;
        fl_color(255, 210, 70);
        fl_pie(
            static_cast<int>(bulletScreenX - bulletRadius),
            static_cast<int>(bulletScreenY - bulletRadius),
            static_cast<int>(pBullet->getWidth()),
            static_cast<int>(pBullet->getHeight()),
            0.0,
            360.0);
    }

    fl_pop_clip();
}

int GameView::handle(int event) {
    switch (event) {
        case FL_FOCUS:
        case FL_UNFOCUS:
            return 1;
        case FL_ENTER:
            take_focus();
            return 1;
        case FL_MOVE:
        case FL_DRAG:
        case FL_PUSH:
        case FL_RELEASE:
            if (mp_inputController != nullptr) {
                const float localX = static_cast<float>(Fl::event_x() - x());
                const float localY = static_cast<float>(Fl::event_y() - y());
                mp_inputController->setMouseLocal(localX, localY);
            }
            if (mp_inputController != nullptr && event == FL_PUSH && Fl::event_button() == FL_LEFT_MOUSE) {
                mp_inputController->setPrimaryFireHeld(true);
            }
            if (mp_inputController != nullptr && event == FL_RELEASE && Fl::event_button() == FL_LEFT_MOUSE) {
                mp_inputController->setPrimaryFireHeld(false);
            }
            return 1;
        case FL_KEYDOWN:
        case FL_SHORTCUT:
            if (mp_inputController != nullptr) {
                mp_inputController->handleKey(Fl::event_key(), true);
            }
            if (Fl::event_key() == FL_Escape && mp_escapeHandler != nullptr) {
                mp_escapeHandler(mp_escapeData);
                return 1;
            }
            return 1;
        case FL_KEYUP:
            if (mp_inputController != nullptr) {
                mp_inputController->handleKey(Fl::event_key(), false);
            }
            return 1;
        default:
            break;
    }

    return Fl_Group::handle(event);
}
