#pragma once

#include "model/bonus.h"

class AmmoPack : public Bonus {
public:
    explicit AmmoPack(float positionX, float positionY);
    ~AmmoPack() override;

    AmmoPack(const AmmoPack&) = delete;
    AmmoPack& operator=(const AmmoPack&) = delete;
    AmmoPack(AmmoPack&&) = delete;
    AmmoPack& operator=(AmmoPack&&) = delete;

    [[nodiscard]] BonusKind getKind() const override;

private:
    void onApplyPlayer(Player& rPlayer) override;
    void onApplyBot(Bot& rBot) override;
};
