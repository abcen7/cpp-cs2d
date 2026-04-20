#include "controller/input_controller.h"

#include "model/game_map.h"
#include "model/game_state.h"
#include "model/player.h"

#include <FL/Enumerations.H>

#include <cmath>

namespace {

bool isKeyMatch(int key, int lowerAscii, int upperAscii) {
    return key == lowerAscii || key == upperAscii;
}

bool isMoveUpKey(int key) {
    return key == FL_Up || isKeyMatch(key, 'w', 'W') || key == static_cast<int>(U'ц') || key == static_cast<int>(U'Ц');
}

bool isMoveLeftKey(int key) {
    return key == FL_Left || isKeyMatch(key, 'a', 'A') || key == static_cast<int>(U'ф') || key == static_cast<int>(U'Ф');
}

bool isMoveDownKey(int key) {
    return key == FL_Down || isKeyMatch(key, 's', 'S') || key == static_cast<int>(U'ы') || key == static_cast<int>(U'Ы');
}

bool isMoveRightKey(int key) {
    return key == FL_Right || isKeyMatch(key, 'd', 'D') || key == static_cast<int>(U'в') || key == static_cast<int>(U'В');
}

} // namespace

void InputController::reset(float viewWidth, float viewHeight) {
    m_keyW = false;
    m_keyA = false;
    m_keyS = false;
    m_keyD = false;
    m_mouseLocalX = viewWidth * 0.5f;
    m_mouseLocalY = viewHeight * 0.5f;
    m_primaryFireHeld = false;
}

void InputController::setPlayer(Player* pPlayer) {
    mp_player = pPlayer;
}

void InputController::handleKey(int key, bool isDown) {
    if (isMoveUpKey(key)) {
        m_keyW = isDown;
    } else if (isMoveLeftKey(key)) {
        m_keyA = isDown;
    } else if (isMoveDownKey(key)) {
        m_keyS = isDown;
    } else if (isMoveRightKey(key)) {
        m_keyD = isDown;
    }

    if (!isDown) {
        return;
    }

    if (mp_player == nullptr) {
        return;
    }

    if (isKeyMatch(key, 'r', 'R')) {
        mp_player->beginReloadActiveWeapon();
    } else if (key == '1') {
        mp_player->switchWeaponSlot(0);
    } else if (key == '2') {
        mp_player->switchWeaponSlot(1);
    } else if (key == '3') {
        mp_player->switchWeaponSlot(2);
    }
}

void InputController::setMouseLocal(float localX, float localY) {
    m_mouseLocalX = localX;
    m_mouseLocalY = localY;
}

void InputController::setPrimaryFireHeld(bool held) {
    m_primaryFireHeld = held;
}

void InputController::updateAimFromMouse(Player& player, float viewWidth, float viewHeight) {
    const float centerX = viewWidth * 0.5f;
    const float centerY = viewHeight * 0.5f;
    const float dx = m_mouseLocalX - centerX;
    const float dy = m_mouseLocalY - centerY;
    if (dx * dx + dy * dy > 4.0f) {
        player.setAimAngleRadians(std::atan2(dy, dx));
    }
}

void InputController::tick(
    Player& player,
    GameState& gameState,
    const GameMap& map,
    float deltaSeconds,
    float viewWidth,
    float viewHeight) {
    player.updateWeapons(deltaSeconds);

    updateAimFromMouse(player, viewWidth, viewHeight);

    const bool isMoving = m_keyW || m_keyA || m_keyS || m_keyD;
    player.setMovingForSpread(isMoving);

    float moveX = 0.0f;
    float moveY = 0.0f;
    if (m_keyW) {
        moveY -= 1.0f;
    }
    if (m_keyS) {
        moveY += 1.0f;
    }
    if (m_keyA) {
        moveX -= 1.0f;
    }
    if (m_keyD) {
        moveX += 1.0f;
    }

    const float length = std::sqrt(moveX * moveX + moveY * moveY);
    if (length > 0.0f) {
        moveX /= length;
        moveY /= length;
    }

    const float speed = player.getSpeed();
    player.tryMoveWithWallCollision(map, moveX * speed * deltaSeconds, moveY * speed * deltaSeconds);

    if (m_primaryFireHeld) {
        player.tryFireWeapon(gameState, map);
    }
}
