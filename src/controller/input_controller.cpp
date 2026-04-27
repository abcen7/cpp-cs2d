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
    std::lock_guard<std::mutex> lock(m_inputMutex);
    m_keyW = false;
    m_keyA = false;
    m_keyS = false;
    m_keyD = false;
    m_mouseLocalX = viewWidth * 0.5f;
    m_mouseLocalY = viewHeight * 0.5f;
    m_primaryFireHeld = false;
    m_reloadRequested = false;
    m_requestedWeaponSlot = -1;
}

void InputController::setPlayer(Player* pPlayer) {
    (void)pPlayer;
}

void InputController::handleKey(int key, bool isDown) {
    std::lock_guard<std::mutex> lock(m_inputMutex);
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

    if (isKeyMatch(key, 'r', 'R')) {
        m_reloadRequested = true;
    } else if (key == '1') {
        m_requestedWeaponSlot = 0;
    } else if (key == '2') {
        m_requestedWeaponSlot = 1;
    } else if (key == '3') {
        m_requestedWeaponSlot = 2;
    }
}

void InputController::setMouseLocal(float localX, float localY) {
    std::lock_guard<std::mutex> lock(m_inputMutex);
    m_mouseLocalX = localX;
    m_mouseLocalY = localY;
}

void InputController::setPrimaryFireHeld(bool held) {
    std::lock_guard<std::mutex> lock(m_inputMutex);
    m_primaryFireHeld = held;
}

void InputController::updateAimFromMouse(Player& player, float mouseLocalX, float mouseLocalY, float viewWidth, float viewHeight) {
    const float centerX = viewWidth * 0.5f;
    const float centerY = viewHeight * 0.5f;
    const float dx = mouseLocalX - centerX;
    const float dy = mouseLocalY - centerY;
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
    bool keyW = false;
    bool keyA = false;
    bool keyS = false;
    bool keyD = false;
    float mouseLocalX = 0.0f;
    float mouseLocalY = 0.0f;
    bool primaryFireHeld = false;
    bool reloadRequested = false;
    int requestedWeaponSlot = -1;
    {
        std::lock_guard<std::mutex> lock(m_inputMutex);
        keyW = m_keyW;
        keyA = m_keyA;
        keyS = m_keyS;
        keyD = m_keyD;
        mouseLocalX = m_mouseLocalX;
        mouseLocalY = m_mouseLocalY;
        primaryFireHeld = m_primaryFireHeld;
        reloadRequested = m_reloadRequested;
        requestedWeaponSlot = m_requestedWeaponSlot;
        m_reloadRequested = false;
        m_requestedWeaponSlot = -1;
    }

    player.updateWeapons(deltaSeconds);

    if (reloadRequested) {
        player.beginReloadActiveWeapon();
    }
    if (requestedWeaponSlot >= 0) {
        player.switchWeaponSlot(requestedWeaponSlot);
    }

    updateAimFromMouse(player, mouseLocalX, mouseLocalY, viewWidth, viewHeight);

    const bool isMoving = keyW || keyA || keyS || keyD;
    player.setMovingForSpread(isMoving);

    float moveX = 0.0f;
    float moveY = 0.0f;
    if (keyW) {
        moveY -= 1.0f;
    }
    if (keyS) {
        moveY += 1.0f;
    }
    if (keyA) {
        moveX -= 1.0f;
    }
    if (keyD) {
        moveX += 1.0f;
    }

    const float length = std::sqrt(moveX * moveX + moveY * moveY);
    if (length > 0.0f) {
        moveX /= length;
        moveY /= length;
    }

    const float speed = player.getSpeed();
    player.tryMoveWithWallCollision(map, moveX * speed * deltaSeconds, moveY * speed * deltaSeconds);

    if (primaryFireHeld) {
        player.tryFireWeapon(gameState, map);
    }
}
