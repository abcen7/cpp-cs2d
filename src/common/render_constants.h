#pragma once

#include <cmath>

/// Must match the rotation applied in `GameView::drawRotatedTexture` when drawing character PNGs.
/// Kenney Top-Down Shooter `*_gun` sprites are right-facing in source texture,
/// so no additional fixed 90deg correction is needed.
inline constexpr float kCharacterSpriteAimOffsetRadians = 0.0f;

/// Barrel tip in **unrotated** texture space (pixels from sprite center, y down), Kenney `*_gun` ~64px.
inline constexpr float kMuzzleTextureOffsetX = 28.0f;
inline constexpr float kMuzzleTextureOffsetY = 5.0f;

/// World position of barrel tip: same 2D rotation as `drawRotatedTexture` (`aim + kCharacterSpriteAimOffsetRadians`).
inline void characterMuzzleWorldPosition(float aimRadians, float centerX, float centerY, float& outX, float& outY) {
    const float th = aimRadians + kCharacterSpriteAimOffsetRadians;
    const float c = std::cos(th);
    const float s = std::sin(th);
    const float tx = kMuzzleTextureOffsetX;
    const float ty = kMuzzleTextureOffsetY;
    outX = centerX + c * tx - s * ty;
    outY = centerY + s * tx + c * ty;
}
