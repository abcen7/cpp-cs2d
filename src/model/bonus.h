#pragma once

#include "model/game_object.h"

class Bot;
class Player;

enum class BonusKind { Health, Ammo, Armor };

class Bonus : public GameObject {
public:
    ~Bonus() override;

    Bonus(const Bonus&) = delete;
    Bonus& operator=(const Bonus&) = delete;
    Bonus(Bonus&&) = delete;
    Bonus& operator=(Bonus&&) = delete;

    [[nodiscard]] virtual BonusKind getKind() const = 0;
    [[nodiscard]] bool isActive() const;

    void applyToPlayer(Player& rPlayer);
    void applyToBot(Bot& rBot);

    void updateModel(float deltaSeconds) override;

protected:
    Bonus(float positionX, float positionY, float width, float height, float lifetimeSeconds);
    void deactivate();

private:
    virtual void onApplyPlayer(Player& rPlayer) = 0;
    virtual void onApplyBot(Bot& rBot) = 0;

    bool m_isActive = true;
    float m_timeRemainingSeconds = 0.0f;
};
