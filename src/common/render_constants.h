#pragma once

#include <cmath>

/// Смещение угла должно совпадать с поворотом спрайта при отрисовке,
/// иначе направление спрайта и баллистика будут расходиться.
inline constexpr float kCharacterSpriteAimOffsetRadians = 0.0f;

/// Смещение дула в неповернутой текстуре (в пикселях от центра спрайта).
inline constexpr float kMuzzleTextureOffsetX = 28.0f;
inline constexpr float kMuzzleTextureOffsetY = 5.0f;

/// Переносит смещение дула в мировые координаты тем же поворотом, что и отрисовка спрайта.
inline void characterMuzzleWorldPosition(float aimRadians, float centerX, float centerY, float& outX, float& outY) {
    const float th = aimRadians + kCharacterSpriteAimOffsetRadians;
    const float c = std::cos(th);
    const float s = std::sin(th);
    const float tx = kMuzzleTextureOffsetX;
    const float ty = kMuzzleTextureOffsetY;
    outX = centerX + c * tx - s * ty;
    outY = centerY + s * tx + c * ty;
}
