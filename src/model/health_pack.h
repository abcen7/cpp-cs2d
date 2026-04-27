#pragma once

#include "model/bonus.h"

class HealthPack : public Bonus {
public:
    explicit HealthPack(float positionX, float positionY);
    ~HealthPack() override;

    HealthPack(const HealthPack&) = delete;
    HealthPack& operator=(const HealthPack&) = delete;
    HealthPack(HealthPack&&) = delete;
    HealthPack& operator=(HealthPack&&) = delete;

    [[nodiscard]] BonusKind getKind() const override;

private:
    void onApplyPlayer(Player& rPlayer) override;
    void onApplyBot(Bot& rBot) override;
};
